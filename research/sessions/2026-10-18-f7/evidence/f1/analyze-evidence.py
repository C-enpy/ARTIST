import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET

evidence = Path(__file__).resolve().parent
root = evidence.parents[2]

def discovery(phase):
    data = json.loads((evidence / f"{phase}-discovery.json").read_text())
    names = [test["name"] for test in data["tests"]]
    disabled = [test["name"] for test in data["tests"]
                if any(prop["name"] == "DISABLED" and prop["value"]
                       for prop in test.get("properties", []))]
    assert not disabled, disabled
    assert len(names) == len(set(names))
    return names

def junit(filename, expected_tests, expected_failures):
    tree = ET.parse(evidence / filename).getroot()
    cases = tree.findall(".//testcase")
    failures = sum(len(case.findall("failure")) for case in cases)
    skipped = sum(len(case.findall("skipped")) for case in cases)
    assert len(cases) == expected_tests
    assert failures == expected_failures
    assert skipped == 0
    assert tree.attrib["tests"] == str(expected_tests)
    assert tree.attrib["failures"] == str(expected_failures)
    assert tree.attrib["disabled"] == "0"
    assert not any(case.findall("error") for case in cases)
    return {"tests": len(cases), "failures": failures, "skipped": skipped,
            "disabled": 0, "names": [case.attrib["name"] for case in cases]}

baseline = discovery("baseline")
red = discovery("red")
green = discovery("green")
targeted = [name for name in green if "AttributeContextTests." in name]
assert len(baseline) == 249
assert len(red) == len(green) == 251
assert red == green
assert set(green) - set(baseline) == set(targeted)
assert not set(baseline) - set(green)
assert len(targeted) == 2
red_run = junit("red-junit.xml", 2, 2)
green_targeted = junit("green-targeted-junit.xml", 2, 0)
green_unit = junit("green-unit-junit.xml", 251, 0)
assert set(red_run["names"]) == set(green_targeted["names"]) == set(targeted)
assert set(green_unit["names"]) == set(green)

red_identity = json.loads((evidence / "red-identity.json").read_text(encoding="utf-8-sig"))
green_identity = json.loads((evidence / "green-identity.json").read_text(encoding="utf-8-sig"))
assert red_identity["files"][1] == green_identity["files"][1]
assert red_identity["files"][0]["sha256"] != green_identity["files"][0]["sha256"]
for identity in green_identity["files"]:
    assert hashlib.sha256((root / identity["path"]).read_bytes()).hexdigest().upper() == identity["sha256"]

summary = {
    "baseline_discovered": len(baseline), "red_discovered": len(red),
    "green_discovered": len(green), "added_names": targeted,
    "red": red_run, "green_targeted": green_targeted,
    "green_unit": {key: value for key, value in green_unit.items() if key != "names"},
    "test_unchanged_between_red_and_green": True,
    "tested_files_match_current_files": True,
    "native_config": "Windows x64 Debug MSVC 14.44; no GL/ASan campaign",
}
(evidence / "validation-summary.json").write_text(json.dumps(summary, indent=2) + "\n")
print(json.dumps(summary, indent=2))
