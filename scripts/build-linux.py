#!/usr/bin/env python3
"""Thin Linux adapter; install the pinned build requirements in a venv first."""
import argparse
from datetime import datetime, timezone
import os
from pathlib import Path
import re
import subprocess
import sys
import uuid

from verify_asan import CANONICAL_ASAN_OPTIONS


def jobs(value):
    count = int(value)
    if not 1 <= count <= 64:
        raise argparse.ArgumentTypeError('jobs must be between 1 and 64')
    return count


def target(value):
    if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_.+-]*', value):
        raise argparse.ArgumentTypeError('target must be one explicit CMake target name')
    return value


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--configuration', choices=('debug', 'release'), default='debug')
    parser.add_argument('--jobs', type=jobs, default=1)
    parser.add_argument('--test-label', choices=('all', 'unit', 'opengl'), default='all')
    parser.add_argument('--configure-only', action='store_true')
    parser.add_argument('--build-only', action='store_true')
    parser.add_argument('--target', type=target, action='append')
    parser.add_argument('--verification-workloads', action='store_true')
    parser.add_argument('--address-sanitizer', action='store_true')
    args = parser.parse_args(argv)
    if args.address_sanitizer and args.configuration != 'debug':
        parser.error('--address-sanitizer requires --configuration debug')
    if args.build_only and args.configure_only:
        parser.error('--build-only and --configure-only are mutually exclusive')
    if args.target and not args.build_only:
        parser.error('--target requires --build-only')
    if args.build_only and not args.target:
        parser.error('--build-only requires at least one --target')
    if args.verification_workloads and (not args.configure_only or args.build_only):
        parser.error('--verification-workloads requires --configure-only without --build-only')
    root = Path(__file__).resolve().parents[1]
    preset = 'linux-' + args.configuration + ('-asan' if args.address_sanitizer else '')
    build_type = args.configuration.capitalize()
    build = root / 'out' / 'build' / preset
    profile = str(root / 'conan' / 'profiles' / 'linux-gcc')
    env = os.environ.copy()
    env['CONAN_HOME'] = str(root / 'out' / 'conan2-linux')
    env['CMAKE_BUILD_PARALLEL_LEVEL'] = str(args.jobs)
    if args.address_sanitizer:
        env['ASAN_OPTIONS'] = CANONICAL_ASAN_OPTIONS
        env.pop('ASAN_ACTIVATION_OPTIONS', None)

    def run(command):
        subprocess.run(command, cwd=root, env=env, check=True)

    try:
        if args.build_only:
            run(['cmake', '--build', '--preset', preset, '--target',
                 *dict.fromkeys(args.target), '--parallel', str(args.jobs)])
            return 0
        run(['conan', 'install', '.', '-pr:h', profile, '-pr:b', profile,
             '-s:h', f'build_type={build_type}', '-s:b', f'build_type={build_type}',
             '-c:h', f'tools.build:jobs={args.jobs}', '-of', str(build), '--build=missing'])
        configure = ['cmake', '--fresh', '--preset', preset]
        if args.verification_workloads:
            configure.append('-DARTIST_BUILD_VERIFICATION=ON')
        run(configure)
        if args.configure_only:
            return 0
        run_id = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid.uuid4().hex
        logs = build / 'runs' / run_id
        logs.mkdir(parents=True)
        if args.address_sanitizer:
            run(['cmake', '--build', '--preset', preset, '--target', 'artist_asan_probe',
                 '--parallel', str(args.jobs)])
            run([sys.executable, str(root / 'scripts' / 'verify_asan.py'),
                 str(build / 'artist_asan_probe'), '--log-dir', str(logs / 'asan-preflight')])
        run(['cmake', '--build', '--preset', preset, '--parallel', str(args.jobs)])
        command = ['ctest', '--preset', preset, '--output-junit', str(logs / 'ctest.xml')]
        if args.test_label != 'all':
            command += ['-L', args.test_label]
        run(command)
        return 0
    except subprocess.CalledProcessError as error:
        print(f'{error.cmd[0]} failed with exit code {error.returncode}', file=sys.stderr)
        return error.returncode if error.returncode > 0 else 1
    except OSError as error:
        print(str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
