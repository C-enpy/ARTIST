"""T4a/e and T4d upload separation: bounded YAML shape + independent review.

No YAML dependency is assumed or installed. This is deliberately not a GitHub
Actions emulator; the accepted inline-list spelling is a private test convention.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[3]
WORKFLOW = ROOT / '.github/workflows/verification-pipeline.yml'


def validate(text):
    lines = [line for line in text.splitlines() if line.strip() and not line.lstrip().startswith('#')]
    assert not any('\t' in line for line in lines), 'tabs require YAML review'
    assert not any(re.search(r'(^|\s)[&*][A-Za-z_]', line) for line in lines), 'anchors not supported'
    # Restrict the event mapping, not a substring anywhere in a comment or string.
    start = lines.index('on:')
    stop = next((i for i in range(start + 1, len(lines)) if not lines[i].startswith(' ')), len(lines))
    events = [line.strip().split(':')[0] for line in lines[start + 1:stop]
              if line.startswith('  ') and not line.startswith('   ')]
    assert events == ['workflow_dispatch'], 'B5: dispatch must be the only trigger'
    expected = {'platform': ['windows', 'linux'],
                'profile': ['cpu-only', 'opengl-software', 'opengl-hardware'],
                'phase': ['list', 'plan', 'prepare', 'build', 'run', 'report'],
                'configuration': ['debug', 'release'],
                'scenario': ['cpu.float-mirror', 'gl.float-routes-correctness',
                             'gl.float-preparation', 'gl.float-lifetime', 'gl.float-submission-cpu']}
    for name, choices in expected.items():
        match = re.search(r'^      ' + name + r':\s*\n((?:        [^\n]*\n?)*)', text, re.M)
        assert match, 'missing bounded input ' + name
        block = match.group(1)
        assert re.search(r'^        type: choice\s*$', block, re.M), 'unbounded input ' + name
        option = re.search(r'^        options: \[([^\]]*)\]\s*$', block, re.M)
        assert option and [v.strip() for v in option.group(1).split(',')] == choices, 'choices ' + name
    runners = [line.strip() for line in lines if line.strip().startswith('runs-on:')]
    assert sorted(runners) == sorted(['runs-on: [self-hosted, Windows, X64]',
                                       'runs-on: [self-hosted, Linux, X64]']), 'B5: prepared self-hosted only'
    for platform in ('windows', 'linux'):
        assert "if: ${{ inputs.platform == '" + platform + "' }}" in text, 'platform job guard'
    commands = [line.strip()[5:].strip() for line in lines if line.strip().startswith('run:')]
    assert sorted(commands) == ['python -B scripts/verification_workflow.py',
                                'python3 -B scripts/verification_workflow.py'], 'B6: only literal transport launch'
    assert not any('continue-on-error:' in line or '|| true' in line for line in lines), 'codes masked'
    uses = [line.strip().split('uses:', 1)[1].strip() for line in lines if 'uses:' in line]
    assert sorted(uses) == sorted(['actions/checkout@v4', 'actions/upload-artifact@v4'] * 2), 'explicit existing action versions only'
    assert text.count('if: always()') == 2, 'upload must retain available failure evidence'
    assert text.count('path: out/verification/') == 2, 'fixed evidence population'
    assert text.count('if-no-files-found: warn') == 2, 'upload absence is not a test verdict'
    assert 'contents: read' in text and 'clean: false' in text, 'read-only checkout preserving predecessor bundles'
    for key in ('PLATFORM', 'PROFILE', 'PHASE', 'CONFIGURATION', 'SCENARIO', 'BUNDLE',
                'EXPECTED_GL_VENDOR', 'EXPECTED_GL_RENDERER', 'ALLOW_DEPENDENCY_RESOLUTION'):
        assert re.search(r'^  VERIFY_' + key + r': \$\{\{ inputs\.[a-z_]+ \}\}\s*$', text, re.M), 'input must travel through env: ' + key
    assert 'VERIFY_OUTPUT_ROOT: out/verification' in text, 'fixed output root'


class WorkflowContractTests(unittest.TestCase):
    def test_real_workflow(self):
        """T4a/e: missing artifact is an assertion, not FileNotFoundError RED."""
        self.assertTrue(WORKFLOW.is_file(), 'B5: manual workflow missing (expected initial RED)')
        validate(WORKFLOW.read_text(encoding='utf-8'))

    def test_mutations_against_real_workflow(self):
        self.assertTrue(WORKFLOW.is_file(), 'B5: manual workflow missing (expected initial RED)')
        source = WORKFLOW.read_text(encoding='utf-8')
        validate(source)
        mutations = [
            ('on:\n', 'on:\n  push:\n'),
            ('on:\n', 'on:\n  pull_request:\n'),
            ('on:\n', 'on:\n  schedule:\n'),
            ('on:\n', 'on:\n  workflow_call:\n'),
            ('runs-on: [self-hosted, Linux, X64]', 'runs-on: ubuntu-latest'),
            ('runs-on: [self-hosted, Windows, X64]', 'runs-on: ${{ inputs.runner }}'),
            ('python3 -B scripts/verification_workflow.py', 'python3 -B scripts/verification_workflow.py ${{ inputs.bundle }}'),
            ('actions/checkout@v4', 'actions/checkout@main'),
            ('if: always()', 'if: success()'),
            ('path: out/verification/', 'path: ${{ inputs.bundle }}'),
            ('options: [windows, linux]', 'options: [windows, linux, cloud]'),
        ]
        for before, after in mutations:
            with self.subTest(before=before):
                self.assertIn(before, source, 'mutation must actually affect source')
                with self.assertRaises((AssertionError, ValueError)):
                    validate(source.replace(before, after, 1))
