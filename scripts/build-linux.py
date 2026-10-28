#!/usr/bin/env python3
"""Thin Linux adapter; install the pinned build requirements in a venv first."""
import argparse
from datetime import datetime, timezone
import os
from pathlib import Path
import subprocess
import sys
import uuid

from verify_asan import CANONICAL_ASAN_OPTIONS


def jobs(value):
    count = int(value)
    if not 1 <= count <= 64:
        raise argparse.ArgumentTypeError('jobs must be between 1 and 64')
    return count


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=jobs, default=1)
    parser.add_argument('--test-label', choices=('all', 'unit', 'opengl'), default='all')
    parser.add_argument('--configure-only', action='store_true')
    parser.add_argument('--address-sanitizer', action='store_true')
    args = parser.parse_args(argv)
    root = Path(__file__).resolve().parents[1]
    preset = 'linux-debug' + ('-asan' if args.address_sanitizer else '')
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
        run(['conan', 'install', '.', '-pr:h', profile, '-pr:b', profile,
             '-c:h', f'tools.build:jobs={args.jobs}', '-of', str(build), '--build=missing'])
        run(['cmake', '--fresh', '--preset', preset])
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
