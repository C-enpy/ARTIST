import datetime
import hashlib
import json
from pathlib import Path
import subprocess
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[3]
artifacts = root / 'out/mission-f7/te'
build = Path('C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build')

def git(*args):
    return subprocess.check_output(['git', *args], cwd=root, text=True).strip()

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def accounting(name):
    xml = ET.parse(artifacts / name).getroot()
    cases = xml.findall('.//testcase')
    names = [c.attrib['name'] for c in cases]
    return {
        'cases': len(cases), 'distinct': len(set(names)),
        'failures': sum(c.find('failure') is not None for c in cases),
        'skips': sum(c.find('skipped') is not None for c in cases),
        'names': names, 'sha256': sha(artifacts / name),
    }

paths = git('ls-files', 'ARTIST/include', 'ARTIST/tests', 'ARTIST/component-tests').splitlines()
aggregate = hashlib.sha256()
for path in paths:
    aggregate.update(path.encode() + b'\0')
    aggregate.update((root / path).read_bytes() + b'\0')

discovered = json.loads((artifacts / 'discovery-final.json').read_text(encoding='utf-8-sig'))
unit_names = sorted(t['name'] for t in discovered['tests'])
unit = accounting('final-unit.junit.xml')
selected = accounting('final-selected.junit.xml')
negative = accounting('oracle-negative.junit.xml')
assert (unit['cases'], unit['distinct'], unit['failures'], unit['skips']) == (226, 226, 0, 0)
assert (selected['cases'], selected['distinct'], selected['failures'], selected['skips']) == (3, 3, 0, 0)
assert (negative['cases'], negative['distinct'], negative['failures'], negative['skips']) == (1, 1, 1, 0)
assert unit_names == sorted(unit['names'])
assert all(n in unit_names for n in selected['names'])
assert not git('diff', '17019e4', '--name-only', '--', 'ARTIST/include', 'ARTIST/tests/mock', 'CMakeLists.txt', 'CMakePresets.json', 'test-datas')

result = {
    'generated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'task': 'task_mvco4dky_000w_30m1nv', 'profile_substitution': 'cpp-engineer for test-engineer, lead explicit',
    'worktree': str(root), 'build': str(build),
    'base': git('rev-parse', '17019e4'), 'head': git('rev-parse', 'HEAD'),
    'branch': git('branch', '--show-current'), 'status': git('status', '--porcelain=v1'),
    'source_test_files': len(paths), 'source_test_sha256': aggregate.hexdigest(),
    'production_mock_config_fixture_delta': [],
    'source_test_hashes': {p: sha(root / p) for p in paths},
    'final_binary_sha256': sha(build / 'ARTIST/tests/ArtistLib_tests.exe'),
    'discovered_unit': len(unit_names),
    'campaigns': {'negative_oracle_only': negative, 'final_selected': selected, 'final_unit': unit},
    'ctest_output_limits': {'passed': 65536, 'failed': 65536},
    'native_gl_executed': 0, 'asan_executed': 0,
    'artifact_hashes': {str(p.relative_to(artifacts)): sha(p) for p in sorted(artifacts.rglob('*')) if p.is_file() and p.name != 'manifest.json'},
}
(artifacts / 'manifest.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({k: result[k] for k in ['head', 'status', 'source_test_sha256', 'final_binary_sha256', 'discovered_unit', 'production_mock_config_fixture_delta']}))
for name in ['negative_oracle_only', 'final_selected', 'final_unit']:
    print(name, {k: v for k, v in result['campaigns'][name].items() if k != 'names'})
