"""Owned finite fake child: arguments are data, never commands or code."""

import json
import os
from pathlib import Path
import sys
import time


def write_all(fd, data):
    remaining = memoryview(data)
    while remaining:
        count = os.write(fd, remaining)
        if count <= 0:
            raise OSError("owned fake stream stopped accepting data")
        remaining = remaining[count:]


def native_fixture(config_file):
    """Finite opt-in mode, with files restricted to the owned config directory."""
    config_path = Path(config_file).resolve(strict=True)
    root = config_path.parent
    if config_path.name != "native-case.json":
        raise ValueError("native fixture config must have its fixed name")
    config = json.loads(config_path.read_text(encoding="utf-8"))
    code = config["code"]
    size = config["log_bytes"]
    pause = config["pause_seconds"]
    if type(code) is not int or code not in (0, 17):
        raise ValueError("owned exit code only")
    if type(size) is not int or not 0 <= size <= 262144:
        raise ValueError("owned log bound exceeded")
    if type(pause) is not int or not 0 <= pause <= 5:
        raise ValueError("owned pause bound exceeded")
    names = ("ARTIST_NATIVE_KEEP", "ARTIST_NATIVE_EMPTY", "ARTIST_NATIVE_DROP", "ARTIST_NATIVE_ONLY")
    report = {"pid": os.getpid(), "arguments": sys.argv[3:],
              "environment": {name: {"present": name in os.environ, "value": os.environ.get(name)}
                              for name in names}}
    with (root / "native-child-result.json").open("x", encoding="utf-8") as stream:
        json.dump(report, stream, ensure_ascii=False)
    write_all(1, b"native-out\n" + b"O" * size)
    write_all(2, b"native-err\n" + b"E" * size)
    with (root / "native-ready.txt").open("x", encoding="utf-8") as stream:
        stream.write("both initial streams flushed")
    time.sleep(pause)  # finite; no subprocess, import, command or path from argv
    if pause:
        # A runner that merely waits for natural completion cannot pretend a
        # successful timeout: the partial-log oracle must exclude these bytes.
        write_all(1, b"late-out\n")
        write_all(2, b"late-err\n")
    raise SystemExit(code)


if __name__ == "__main__":
    if len(sys.argv) >= 3 and sys.argv[1] == "__artist_native_fixture_v1__":
        native_fixture(sys.argv[2])
    # Both fixed-prefix arguments are supplied by the reviewed fixture.
    output = Path(sys.argv[1])
    code = int(sys.argv[2])
    with output.open("x", encoding="utf-8") as stream:
        json.dump({"arguments": sys.argv[3:], "code": code}, stream, ensure_ascii=False)
    print("owned verification child", flush=True)
    raise SystemExit(code)
