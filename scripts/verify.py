#!/usr/bin/env python3
"""Portable verification CLI: consultative modes never load platform services."""
import sys

sys.dont_write_bytecode = True

import argparse
import importlib
import json
from pathlib import Path

from verification import catalogue, evidence, planning, reporting
from verification.model import BuildReceipt, PreparationReceipt, Request
from verification.runner import SubprocessRunner


class _Once(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        seen = getattr(namespace, '_seen_options', set())
        if self.dest in seen:
            parser.error('duplicate option: ' + str(option_string))
        seen.add(self.dest)
        namespace._seen_options = seen
        setattr(namespace, self.dest, values)


class _FlagOnce(_Once):
    def __init__(self, option_strings, dest, **kwargs):
        kwargs['default'] = False
        super().__init__(option_strings, dest, nargs=0, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        super().__call__(parser, namespace, True, option_string)


def _parser():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='mode')
    listing = commands.add_parser('list')
    listing.add_argument('--json', action=_FlagOnce)
    for mode in ('plan', 'prepare'):
        command = commands.add_parser(mode)
        for name, choices in (('platform', ('windows', 'linux')),
                              ('profile', ('cpu-only', 'opengl-software', 'opengl-hardware')),
                              ('configuration', ('debug', 'release'))):
            command.add_argument('--' + name, choices=choices, required=True, action=_Once)
        command.add_argument('--scenario', action='append', required=True)
        command.add_argument('--route', action='append', default=[])
        for name in ('iterations', 'repetitions', 'batch-size'):
            command.add_argument('--' + name, type=int, default=1, action=_Once)
        command.add_argument('--measurement-requested', action=_FlagOnce)
        command.add_argument('--expected-gl-vendor', action=_Once)
        command.add_argument('--expected-gl-renderer', action=_Once)
        if mode == 'prepare':
            command.add_argument('--output-root', required=True, action=_Once)
            command.add_argument('--allow-dependency-resolution', action=_FlagOnce)
    for mode in ('build', 'run', 'report'):
        command = commands.add_parser(mode)
        command.add_argument('--bundle', required=True, action=_Once)
        if mode == 'report':
            command.add_argument('--json', action=_FlagOnce)
    return parser


def _request(args):
    return Request(platform=args.platform, profile=args.profile, scenarios=tuple(args.scenario),
                   configuration=args.configuration, routes=tuple(args.route), iterations=args.iterations,
                   repetitions=args.repetitions, batch_size=args.batch_size,
                   measurement_requested=args.measurement_requested,
                   allow_dependency_resolution=getattr(args, 'allow_dependency_resolution', False),
                   expected_gl_vendor=args.expected_gl_vendor, expected_gl_renderer=args.expected_gl_renderer)


def _adapter(platform, source_root):
    # No user-controlled module names, plugins or import during list/plan/report.
    modules = {'windows': ('verification.adapters.windows', 'WindowsAdapter'),
               'linux': ('verification.adapters.linux', 'LinuxAdapter')}
    module_name, class_name = modules[platform]
    try:
        cls = getattr(importlib.import_module(module_name), class_name)
    except (ImportError, AttributeError) as error:
        raise evidence.EvidenceError('platform adapter is not integrated: ' + platform, 3) from error
    return cls(source_root)


def main(argv=None, *, adapter_factory=None, runner=None, source_root=None,
         revision_reader=None, result_parser=None):
    parser = _parser()
    try:
        try:
            args = parser.parse_args(argv)
        except SystemExit as stopped:
            return stopped.code
        if args.mode is None:
            parser.print_help()
            return 0
        if args.mode == 'list':
            specs = catalogue.list_scenarios()
            if args.json:
                print(json.dumps({'schema_version': 1, 'scenarios': [spec.to_dict() for spec in specs]}))
            else:
                print('\n'.join(spec.id for spec in specs))
            return 0
        if args.mode in ('plan', 'prepare'):
            try:
                plan = planning.make_plan(_request(args), catalogue.list_scenarios())
            except (ValueError, TypeError) as error:
                print(str(error), file=sys.stderr)
                return 2
            if args.mode == 'plan':
                print(json.dumps(plan.to_dict(), ensure_ascii=True, allow_nan=False))
                return 0
        if args.mode == 'report':
            result = reporting.report(args.bundle)
            print(json.dumps(result, ensure_ascii=True, allow_nan=False))
            return result['exit_code']
        # An injected adapter is a service composition, never an excuse to
        # silently substitute a real process runner when its double is missing.
        if adapter_factory is not None and runner is None:
            raise evidence.EvidenceError('injected adapters require an explicit runner', 2)
        process_runner = runner if runner is not None else SubprocessRunner()
        if args.mode == 'prepare':
            root = Path(source_root).resolve() if source_root is not None else Path(__file__).resolve().parents[1]
            adapter = adapter_factory(plan.request.platform) if adapter_factory is not None else _adapter(plan.request.platform, root)
            sink = evidence.BundleSink(root, args.output_root, runner=process_runner,
                                       revision_reader=revision_reader, result_parser=result_parser)
            receipt = evidence.prepare(plan, adapter, sink)
        else:
            sink = evidence.BundleSink.load(args.bundle, runner=process_runner,
                                            revision_reader=revision_reader, result_parser=result_parser)
            plan = sink.plan
            if source_root is not None and Path(source_root).resolve() != sink.source_root:
                raise evidence.EvidenceError('bundle source root differs from the requested candidate', stale=True)
            phase, cls = ('prepare', PreparationReceipt) if args.mode == 'build' else ('build', BuildReceipt)
            if phase not in sink.manifest['receipts']:
                raise evidence.EvidenceError('required predecessor receipt is absent', 3)
            predecessor = cls.from_dict(sink.manifest['receipts'][phase])
            evidence._predecessor(sink, plan, predecessor, phase)
            missing_target = (args.mode == 'run' and plan.request.profile == 'opengl-hardware'
                              and plan.request.expected_gl_vendor is None)
            # Even adapter construction can observe a device. Missing-target
            # execution is recorded by the core without loading that service.
            blocked_hardware = missing_target or (args.mode == 'run' and evidence._unsupported_hardware_ctest(plan))
            adapter = None if blocked_hardware else (adapter_factory(plan.request.platform)
                       if adapter_factory is not None else _adapter(plan.request.platform, sink.source_root))
            receipt = (evidence.build(plan, predecessor, adapter, sink) if args.mode == 'build' else
                       evidence.execute(plan, predecessor, adapter, sink))
        print(json.dumps({'bundle': str(sink.bundle), 'receipt': receipt.to_dict()}, ensure_ascii=True, allow_nan=False))
        return receipt.details['exit_code']
    except evidence.EvidenceError as error:
        print(str(error), file=sys.stderr)
        return error.exit_code
    except KeyboardInterrupt:
        print('verification interrupted', file=sys.stderr)
        return 130
    except (ValueError, TypeError) as error:
        print(str(error), file=sys.stderr)
        return 4
    except (OSError, KeyError) as error:
        print(str(error), file=sys.stderr)
        return 4
    except Exception as error:
        print(type(error).__name__ + ': ' + str(error), file=sys.stderr)
        return 4


if __name__ == '__main__':
    raise SystemExit(main())
