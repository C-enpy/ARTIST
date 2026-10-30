"""Read-only reconstruction from current evidence, never from report.json."""
from pathlib import Path
import json
from types import SimpleNamespace
import xml.etree.ElementTree as ET

from . import evidence
from .model import (BuildReceipt, CommandSpec, ExecutionReceipt, Freshness, MeasurementValidity,
                    PipelineState, PreparationReceipt, ProcessOutcome, TestVerdict, VerificationPlan)


def _base():
    return dict(schema_version=1, run_id=None, plan_sha256=None, pipeline_state='BLOCKED',
                test_verdict='NOT_EXECUTED', measurement_validity='ABSENT',
                freshness='UNVERIFIED', exit_code=3, causes=[{'exit_code': 3, 'message': 'bundle evidence absent'}])


def _problem(result, message, code=4, *, stale=False):
    result['causes'].append({'exit_code': code, 'message': message})
    result['exit_code'] = evidence._combined(result['exit_code'], code)
    if stale:
        result['freshness'] = 'STALE'
    if result['exit_code'] == 130:
        result['pipeline_state'] = 'INTERRUPTED'
    elif result['exit_code'] == 4:
        result['pipeline_state'] = 'FAILED'
    elif result['exit_code'] == 3:
        result['pipeline_state'] = 'BLOCKED'
    if code == 4 and result['test_verdict'] == 'PASS':
        result['test_verdict'] = 'INCONCLUSIVE'


def _identity(document, run_id, digest):
    if (type(document.get('schema_version')) is not int or document['schema_version'] != 1
            or document.get('run_id') != run_id or document.get('plan_sha256') != digest):
        raise evidence.EvidenceError('document identity disagrees with bundle', stale=True)


def _completed_proof(root, plan, manifest, receipts):
    artifacts = manifest['artifacts']
    required = ('environment.json', 'identity-before.json', 'identity-build.json',
                'identity-after.json', 'commands.jsonl')
    if any(name not in artifacts for name in required):
        raise evidence.EvidenceError('completed candidate lacks required provenance artifacts')
    for phase in ('prepare', 'build'):
        snapshot = receipts[phase].details.get('identity', {})
        if not snapshot.get('files') or not snapshot.get('head') or type(snapshot.get('delta')) is not str:
            raise evidence.EvidenceError('completed phase lacks source/revision identities')
    executables = receipts['build'].details.get('executables', {})
    targets = {target for spec in plan.scenarios for target in spec.targets}
    if set(executables) != targets or any(relative not in receipts['build'].details['identity']['files']
                                         for relative in executables.values()):
        raise evidence.EvidenceError('completed build lacks its required executable identities')
    records = [json.loads(line) for line in (root / 'commands.jsonl').read_text(encoding='utf-8').splitlines()]
    phases, pairs, junit = set(), [], []
    hardware_ctest = evidence._has_hardware_ctest(plan)
    commands, discovered, returncodes = [], [], []
    for record in records:
        _identity(record, manifest['run_id'], plan.plan_sha256)
        outcome = ProcessOutcome(record['returncode'], '', '', record['timed_out'], record['interrupted'])
        if record['phase'] not in ('prepare', 'build', 'run') or not record['argv']:
            raise evidence.EvidenceError('command provenance is incomplete')
        phases.add(record['phase'])
        for key in ('stdout_log', 'stderr_log'):
            if record[key] not in artifacts:
                raise evidence.EvidenceError('command stream lacks artifact attribution')
        options = evidence._options(record['argv'])
        if hardware_ctest and record['phase'] == 'run':
            if '--output-junit' in options or '--show-only=json-v1' in record['argv']:
                evidence._check_child(outcome, allowed=(0, 8) if '--output-junit' in options else (0,))
            commands.append(CommandSpec(tuple(record['argv']), record['cwd'], record['env_delta'],
                                        record['timeout_seconds'], 'run', tuple(record['declared_effects'])))
            if '--show-only=json-v1' in record['argv']:
                inventory = evidence._json(root / record['stdout_log'])
                names = [test['name'] for test in inventory['tests']]
                if not names or any(type(name) is not str or not name for name in names):
                    raise evidence.EvidenceError('recorded CTest discovery is empty or malformed')
                discovered.extend(names)
        for option in ('--result', '--output-junit'):
            if option in options:
                path = Path(options[option])
                path = path if path.is_absolute() else Path(record['cwd']) / path
                path = evidence._within(root, path)
                if path.relative_to(root).as_posix() not in artifacts:
                    raise evidence.EvidenceError('selected producer result is missing from artifact inventory')
                if option == '--result':
                    pairs.append((options.get('--scenario'), options.get('--route')))
                else:
                    junit.append(path)
                    returncodes.append(record['returncode'])
    expected = [(spec.id, route) for spec in plan.scenarios if spec.producer['kind'] == 'workload'
                for route in (plan.request.routes or spec.routes)]
    if phases != {'prepare', 'build', 'run'} or sorted(pairs) != sorted(expected):
        raise evidence.EvidenceError('completed invocation population differs from selection')
    if any(spec.producer['kind'] == 'selection_ctest' for spec in plan.scenarios) and not junit:
        raise evidence.EvidenceError('selected CTest proof is absent')
    if hardware_ctest:
        # Reconstruct from the attributed command and original case bytes, not
        # report.json, global stdout, a cached device or a new native probe.
        owner = SimpleNamespace(plan=plan, run_id=manifest['run_id'], bundle=root)
        context_requests = evidence._ctest_context_requests(owner, commands)
        context_requests = {evidence._within(root, path): expected for path, expected in context_requests.items()}
        groups = {path: list(ET.parse(path).iter('testcase')) for path in junit}
        cases = [case for group in groups.values() for case in group]
        selected = tuple(dict.fromkeys(name for spec in plan.scenarios
                                      if spec.producer['kind'] == 'selection_ctest'
                                      for name in spec.producer['names']))
        evidence._check_discovery(discovered, selected)
        if sorted(case.attrib.get('name', '') for case in cases) != sorted(selected):
            raise evidence.EvidenceError('recorded CTest population disagrees with selection')
        findings = evidence._hardware_context_findings(groups, context_requests, returncodes)
        failed = any(case.find(tag) is not None for case in cases for tag in ('failure', 'error'))
        proof_code = 5 if failed else 0
        for cause in findings:
            proof_code = evidence._combined(proof_code, cause['exit_code'])
        if (evidence._combined(manifest['exit_code'], proof_code) != manifest['exit_code']
                or (failed and manifest['test_verdict'] != 'FAIL')
                or (findings and manifest['test_verdict'] == 'PASS')):
            raise evidence.EvidenceError('completed verdict contradicts its per-case CTest proof')


def report(bundle):
    result = _base()
    if bundle is None or not Path(bundle).is_dir():
        return result
    root = Path(bundle).resolve()
    try:
        if not (root / 'manifest.json').is_file() or not (root / 'plan.json').is_file():
            return result
        manifest, envelope = evidence._json(root / 'manifest.json'), evidence._json(root / 'plan.json')
        plan = VerificationPlan.from_dict(envelope['plan'])
        run_id, digest = manifest['run_id'], plan.plan_sha256
        if type(run_id) is not str or not run_id:
            raise evidence.EvidenceError('bundle run identity is absent')
        _identity(envelope, run_id, digest)
        _identity(manifest, run_id, digest)
        required = {'pipeline_state', 'test_verdict', 'measurement_validity', 'freshness', 'exit_code',
                    'causes', 'receipts', 'artifacts', 'source_root'}
        if not required.issubset(manifest):
            raise evidence.EvidenceError('manifest is incomplete')
        result = {key: manifest[key] for key in ('schema_version', 'run_id', 'plan_sha256',
                  'pipeline_state', 'test_verdict', 'measurement_validity', 'freshness', 'exit_code')}
        if (not isinstance(manifest['causes'], list) or any(not isinstance(cause, dict)
                or type(cause.get('message')) is not str or type(cause.get('exit_code')) is not int
                for cause in manifest['causes'])):
            raise evidence.EvidenceError('manifest causes are malformed')
        result['causes'] = list(manifest['causes'])
        PipelineState(result['pipeline_state'])
        TestVerdict(result['test_verdict'])
        MeasurementValidity(result['measurement_validity'])
        Freshness(result['freshness'])
        if type(result['exit_code']) is not int or result['exit_code'] not in (0, 3, 4, 5, 6, 130):
            raise evidence.EvidenceError('manifest exit classification is invalid')
        receipts = {}
        for phase, cls in (('prepare', PreparationReceipt), ('build', BuildReceipt), ('run', ExecutionReceipt)):
            if phase in manifest['receipts']:
                receipt = cls.from_dict(manifest['receipts'][phase])
                if receipt.run_id != run_id or receipt.plan_sha256 != digest:
                    raise evidence.EvidenceError('receipt belongs to another candidate', stale=True)
                for relative, expected in receipt.artifacts.items():
                    if manifest['artifacts'].get(relative) != expected:
                        raise evidence.EvidenceError('artifact index contradicts phase receipt: ' + relative, stale=True)
                receipts[phase] = receipt
        execution = receipts.get('run')
        if execution is not None:
            if (execution.state != manifest['pipeline_state']
                    or type(execution.details.get('exit_code')) is not int
                    or execution.details['exit_code'] != manifest['exit_code']
                    or (execution.state != 'COMPLETED' and manifest['test_verdict'] == 'PASS')):
                raise evidence.EvidenceError('manifest contradicts its execution receipt')
        for relative, expected in manifest['artifacts'].items():
            path = evidence._within(root, relative, relative_only=True)
            if not path.is_file() or evidence._sha(path) != expected:
                raise evidence.EvidenceError('raw/log/provenance artifact changed: ' + relative, stale=True)
        for filename in ('identity-before.json', 'identity-build.json', 'identity-after.json', 'environment.json'):
            if (root / filename).is_file():
                _identity(evidence._json(root / filename), run_id, digest)
        # Compare against the accepted pre-execution candidate, not a possibly
        # changed identity-after snapshot. No subprocess or revision probe here.
        baseline = receipts.get('build') or receipts.get('prepare')
        if baseline is not None and baseline.details.get('identity') is not None:
            expected = baseline.details['identity']
            source = Path(manifest['source_root']).resolve()
            current_files = evidence._source_files(source)
            if baseline.phase == 'build':
                try:
                    build_files, _ = evidence._build_files(SimpleNamespace(plan=plan, source_root=source))
                except (evidence.EvidenceError, OSError) as error:
                    raise evidence.EvidenceError('current build population is missing or ambiguous: ' + str(error),
                                                 stale=True) from error
                current_files.update(build_files)
            current_population = {path.relative_to(source).as_posix() for path in current_files}
            if current_population != set(expected['files']):
                raise evidence.EvidenceError('relevant source/configuration/build population changed', stale=True)
            for relative, digest_value in expected['files'].items():
                path = evidence._within(source, relative, relative_only=True)
                if not path.is_file() or evidence._sha(path) != digest_value:
                    raise evidence.EvidenceError('candidate source/config/shader/executable changed: ' + relative, stale=True)
            if evidence._git_files(source) != expected.get('revision_files', {}):
                raise evidence.EvidenceError('candidate revision metadata changed', stale=True)
        if result['pipeline_state'] == 'RUNNING' or (root / '.phase.lock').exists():
            result['pipeline_state'], result['freshness'] = 'RUNNING', 'UNVERIFIED'
            _problem(result, 'phase has not completed', 6)
        elif 'run' not in receipts:
            _problem(result, 'no execution receipt; tests were not executed', 3)
        elif result['pipeline_state'] == 'COMPLETED':
            if not {'prepare', 'build', 'run'}.issubset(receipts) or not (root / 'identity-after.json').is_file():
                raise evidence.EvidenceError('completed manifest lacks its required phase proof')
            if result['test_verdict'] == 'NOT_EXECUTED':
                raise evidence.EvidenceError('completed execution has no test verdict')
            if (any(receipts[phase].state != 'COMPLETED' or receipts[phase].details.get('exit_code') != 0
                    for phase in ('prepare', 'build'))
                    or receipts['run'].state != 'COMPLETED'
                    or receipts['run'].details.get('exit_code') != result['exit_code']):
                raise evidence.EvidenceError('completed manifest disagrees with its phase receipts')
            _completed_proof(root, plan, manifest, receipts)
        return result
    except Exception as error:
        if not isinstance(result.get('causes'), list):
            result['causes'] = []
        for key, enum, fallback in (('pipeline_state', PipelineState, 'FAILED'),
                                   ('test_verdict', TestVerdict, 'INCONCLUSIVE'),
                                   ('measurement_validity', MeasurementValidity, 'ABSENT'),
                                   ('freshness', Freshness, 'UNVERIFIED')):
            try:
                enum(result[key])
            except (TypeError, ValueError, KeyError):
                result[key] = fallback
        _problem(result, str(error), stale=isinstance(error, evidence.EvidenceError) and error.stale)
        return result
