"""Pure validation of one historical OpenGL case's supplied context evidence.

No native workload, environment, filesystem or process is consulted. Validation
attributes supplied data; it does not prove physical acceleration, execute GL or
interpret JUnit assertions/skips and process exit codes.
"""

import json


_PREFIX = "[ARTIST_CONTEXT_V1] "
_EXPECTED_FIELDS = frozenset({
    "run_id", "plan_sha256", "request_id", "ctest_name", "profile",
    "expected_gl_vendor", "expected_gl_renderer",
})
_OBSERVATION_FIELDS = _EXPECTED_FIELDS | {
    "schema_version", "kind", "gl_vendor", "gl_renderer", "gl_version",
    "context_major", "context_minor", "context_profile_mask", "context_current",
    "qualification", "precondition", "reason",
}
_SOFTWARE_RENDERERS = (
    "llvmpipe", "softpipe", "swiftshader", "software", "swrast", "gdi generic",
)


def _string(value, label, *, allow_empty=False, controls=False):
    if type(value) is not str or len(value) > 4096 or (not value and not allow_empty):
        raise ValueError(f"{label} must be text of at most 4096 Unicode scalars")
    for character in value:
        scalar = ord(character)
        if 0xd800 <= scalar <= 0xdfff:
            raise ValueError(f"{label} contains a non-scalar Unicode value")
        if not controls and (scalar < 0x20 or 0x7f <= scalar <= 0x9f):
            raise ValueError(f"{label} contains a control character")


def _expected_identity(expected):
    if type(expected) is not dict or set(expected) != _EXPECTED_FIELDS:
        raise ValueError("expected must contain exactly the seven attribution fields")
    for key in _EXPECTED_FIELDS:
        _string(expected[key], key)
    for key in ("run_id", "request_id"):
        if not expected[key].strip():
            raise ValueError(f"{key} must not be blank")
    digest = expected["plan_sha256"]
    if len(digest) != 64 or any(character not in "0123456789abcdef" for character in digest):
        raise ValueError("plan_sha256 must be a lowercase SHA-256 hex digest")
    name = expected["ctest_name"]
    parts = name.split(".", 2)
    if (len(parts) != 3 or parts[0] != "opengl" or not parts[1] or not parts[2]
            or any(character.isspace() for character in name)):
        raise ValueError("ctest_name must identify an opengl suite and test")
    if expected["profile"] != "opengl-hardware":
        raise ValueError("context observations require the opengl-hardware profile")


def _unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError("duplicate JSON object key")
        result[key] = value
    return result


def _invalid_constant(value):
    raise ValueError("nonstandard JSON constant")


def _observation_types(value):
    if type(value) is not dict or set(value) != _OBSERVATION_FIELDS:
        raise ValueError("observation has missing or unknown fields")
    if type(value["schema_version"]) is not int or value["schema_version"] != 1:
        raise ValueError("unsupported context schema_version")
    if type(value["kind"]) is not str or value["kind"] != "opengl-context":
        raise ValueError("unsupported context observation kind")
    for key in ("gl_vendor", "gl_renderer", "gl_version"):
        if value[key] is not None:
            _string(value[key], key)
    for key in ("context_major", "context_minor", "context_profile_mask"):
        number = value[key]
        if number is not None and (type(number) is not int or not 0 <= number <= 2147483647):
            raise ValueError(f"{key} must be a nonnegative GL integer or null")
    if type(value["context_current"]) is not bool:
        raise ValueError("context_current must be a boolean")
    # A diagnostic is data, including any JSON-escaped line breaks, never a command.
    _string(value["reason"], "reason", allow_empty=True, controls=True)
    if (type(value["qualification"]) is not str
            or value["qualification"] not in ("MATCHED_EXPECTATION", "UNVERIFIED")):
        raise ValueError("unknown context qualification")
    if (type(value["precondition"]) is not str
            or value["precondition"] not in ("OK", "UNAVAILABLE", "INVALID_REQUEST")):
        raise ValueError("unknown context precondition")


def _qualification(value):
    matched = value["qualification"] == "MATCHED_EXPECTATION"
    if matched != (value["precondition"] == "OK"):
        raise ValueError("contradictory qualification and precondition")
    if not matched:
        # Preserve unavailable/invalid-request observations, even with partial
        # metadata or a software/mismatching context. The collector owns verdicts.
        return
    if not value["context_current"]:
        raise ValueError("qualified evidence requires the owned context current")
    for key in ("gl_vendor", "gl_renderer", "gl_version", "context_major",
                "context_minor", "context_profile_mask"):
        if value[key] is None:
            raise ValueError("qualified evidence cannot contain unknown context fields")
    if ((value["context_major"], value["context_minor"]) < (3, 3)
            or value["context_profile_mask"] != 1):
        raise ValueError("qualified evidence requires an unambiguous OpenGL 3.3+ core context")
    if (value["gl_vendor"] != value["expected_gl_vendor"]
            or value["gl_renderer"] != value["expected_gl_renderer"]):
        raise ValueError("observed GL target disagrees with the exact expectation")
    if any(marker in value["gl_renderer"].casefold() for marker in _SOFTWARE_RENDERERS):
        raise ValueError("a known software renderer cannot be qualified as hardware")


def parse_context_observation(text: str, *, expected: dict) -> dict:
    """Validate exactly one attributed observation, or raise ValueError.

    ``text`` is the system-out of one case, not a whole CTest/JUnit population.
    The returned object is freshly decoded on every call; expected is untouched.
    """
    if type(text) is not str:
        raise ValueError("system-out must be text")
    _expected_identity(expected)
    # Only LF/CRLF delimit transport lines. Unicode line separators inside JSON
    # strings remain ordinary scalar data, not extra observation boundaries.
    lines = [line for line in text.split("\n") if line.startswith(_PREFIX)]
    if len(lines) != 1:
        raise ValueError("exactly one context observation is required")
    try:
        value = json.loads(lines[0][len(_PREFIX):], object_pairs_hook=_unique_object,
                           parse_constant=_invalid_constant)
    except RecursionError as error:
        raise ValueError("context JSON is too deeply nested") from error
    _observation_types(value)
    for key in _EXPECTED_FIELDS:
        # Do not coerce types or normalize any identity/target text.
        if type(value[key]) is not str or value[key] != expected[key]:
            raise ValueError(f"context observation has foreign attribution: {key}")
    _qualification(value)
    return value
