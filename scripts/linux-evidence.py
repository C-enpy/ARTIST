#!/usr/bin/env python3
"""CTest evidence adapter shared by WSL and CI; run inside xvfb-run."""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import uuid
import xml.etree.ElementTree as ET

from verify_asan import CANONICAL_ASAN_OPTIONS


def run_logged(command, output, root, env):
    # Exclusive creation prevents an old result becoming evidence after failure.
    with output.open('x', encoding='utf-8') as stream:
        subprocess.run(command, cwd=root, env=env, stdout=stream,
                       stderr=subprocess.STDOUT, check=True)


def reconcile(names, xml):
    cases = list(ET.parse(xml).iter('testcase'))
    actual = [case.attrib['name'] for case in cases]
    if not names or len(set(names)) != len(names) or sorted(actual) != sorted(names):
        raise ValueError('CTest discovery/JUnit population mismatch or empty selection')
    if any(case.find(tag) is not None for case in cases for tag in ('failure', 'error', 'skipped')):
        raise ValueError('CTest failure/error/skip is not a baseline success')
    return len(actual)


def result_summary(names, xml, returncode):
    cases = list(ET.parse(xml).iter('testcase'))
    actual = [case.attrib['name'] for case in cases]
    return {'discovered': len(names), 'executed': len(cases), 'returncode': returncode,
            'population_matches': bool(names) and len(set(names)) == len(names) and sorted(actual) == sorted(names),
            'failures': [case.attrib['name'] for case in cases if case.find('failure') is not None or case.find('error') is not None],
            'skips': [case.attrib['name'] for case in cases if case.find('skipped') is not None]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address-sanitizer', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    preset = 'linux-debug' + ('-asan' if args.address_sanitizer else '')
    build = root / 'out' / 'build' / preset
    run = root / 'out' / 'linux-evidence' / (datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid.uuid4().hex)
    run.mkdir(parents=True)
    env = os.environ.copy()
    env.update(LIBGL_ALWAYS_SOFTWARE='true', GALLIUM_DRIVER='llvmpipe')
    if args.address_sanitizer:
        env['ASAN_OPTIONS'] = CANONICAL_ASAN_OPTIONS
        env.pop('ASAN_ACTIVATION_OPTIONS', None)
    manifest = {'preset': preset, 'status': 'INCONCLUSIVE', 'commands': [], 'counts': {}}

    def execute(command, filename):
        manifest['commands'].append(command)
        run_logged(command, run / filename, root, env)

    def identity():
        paths = ['CMakeLists.txt', 'CMakePresets.json', 'conanfile.txt',
                 'conan/profiles/linux-gcc', 'cmake/AddressSanitizer.cmake',
                 'scripts/build-linux.py', 'scripts/linux-evidence.py', 'scripts/verify_asan.py']
        paths += [str(path.relative_to(root)) for path in (root / 'ARTIST').rglob('*')
                  if path.is_file() and path.suffix in ('.cpp', '.hpp', '.h', '.txt')]
        paths += [str(path.relative_to(root)) for path in build.rglob('ArtistLib*tests') if path.is_file()]
        return {path: hashlib.sha256((root / path).read_bytes()).hexdigest() for path in sorted(paths)}

    try:
        execute(['git', 'rev-parse', 'HEAD'], 'head.txt')
        execute(['git', 'diff', 'HEAD', '--'], 'delta.patch')
        execute(['uname', '-a'], 'uname.txt')
        execute(['g++-14', '--version'], 'compiler.txt')
        execute(['cmake', '--version'], 'cmake.txt')
        execute(['conan', '--version'], 'conan.txt')
        execute(['ninja', '--version'], 'ninja.txt')
        configure = [sys.executable, 'scripts/build-linux.py', '--configure-only']
        if args.address_sanitizer:
            configure.append('--address-sanitizer')
        execute(configure, 'configure.log')
        if args.address_sanitizer:
            execute(['cmake', '--build', '--preset', preset, '--target', 'artist_asan_probe', '--parallel', '1'], 'probe-build.log')
            execute([sys.executable, 'scripts/verify_asan.py', str(build / 'artist_asan_probe'), '--log-dir', str(run / 'canary')], 'canary.log')
        execute(['cmake', '--build', '--preset', preset, '--parallel', '1'], 'build.log')
        before = identity()
        manifest['identities'] = before
        for label in ('unit', 'opengl'):
            execute(['ctest', '--preset', preset, '-L', label, '--show-only=json-v1'], label + '-inventory.json')
            inventory = json.loads((run / (label + '-inventory.json')).read_text())
            names = [test['name'] for test in inventory['tests']]
            xml = run / (label + '.xml')
            if xml.exists():
                raise FileExistsError(xml)
            returncode = 0
            try:
                execute(['ctest', '--preset', preset, '-L', label, '--parallel', '1', '--no-tests=error',
                         '--output-on-failure', '--output-junit', str(xml)], label + '.log')
            except subprocess.CalledProcessError as error:
                returncode = error.returncode
            manifest['counts'][label] = result_summary(names, xml, returncode)
            if returncode:
                raise subprocess.CalledProcessError(returncode, manifest['commands'][-1])
            reconcile(names, xml)
            if label == 'opengl':
                cases = list(ET.parse(xml).iter('testcase'))
                for case in cases:
                    output = case.findtext('system-out', '')
                    context = [line for line in output.splitlines() if line.startswith('[OpenGL]')]
                    if not context or any('llvmpipe' not in line.lower() for line in context):
                        raise ValueError('Required llvmpipe context absent: ' + case.attrib['name'])
        if before != identity():
            raise ValueError('Source/configuration/binary identity changed during tests')
        manifest['status'] = 'PASS'
        return 0
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        manifest['status'] = 'FAIL'
        manifest['error'] = str(error)
        print(error, file=sys.stderr)
        return error.returncode if isinstance(error, subprocess.CalledProcessError) and error.returncode > 0 else 1
    finally:
        (run / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
        print(run)


if __name__ == '__main__':
    sys.exit(main())
