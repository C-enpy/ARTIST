"""T2 JUnit summary (pool-6, not versioned). Usage: python junit.py <file.junit.xml>...

Prints, per file: SHA256, cases, distinct names, failures, skips, and the
name + first message line of every failed or skipped case.
"""
import hashlib
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

for name in sys.argv[1:]:
    path = Path(name)
    digest = hashlib.sha256(path.read_bytes()).hexdigest().upper()
    cases = ET.parse(path).getroot().iter("testcase")
    rows = []
    for case in cases:
        failure = case.find("failure")
        if failure is None:
            failure = case.find("error")
        skipped = case.find("skipped")
        status = case.get("status", "")
        if skipped is not None or status in ("notrun", "disabled"):
            state = "skip"
        elif failure is not None or status == "fail":
            state = "fail"
        else:
            state = "pass"
        text = ""
        if failure is not None:
            text = (failure.get("message") or failure.text or "").strip().splitlines()[:1]
            text = text[0][:160] if text else ""
        rows.append((case.get("name"), state, text))
    names = {row[0] for row in rows}
    fails = [row for row in rows if row[1] == "fail"]
    skips = [row for row in rows if row[1] == "skip"]
    print(f"{path.name} sha256={digest} cases={len(rows)} distinct={len(names)} "
          f"fail={len(fails)} skip={len(skips)}")
    for row in fails + skips:
        print(f"  {row[1].upper()} {row[0]} :: {row[2]}")
