"""Pure, bounded argv and JSON contracts for the uniform workloads.

No native workload, core package, subprocess or filesystem operation is loaded
or performed here. Invocation construction never executes its returned argv.
Parsing validates supplied evidence, not executable identity or actual GL use.
"""

import json
import math


_IDENTITY_FIELDS = {
    "schema_version", "run_id", "request_id", "plan_sha256", "scenario",
    "scenario_version", "route", "profile", "parameters",
}
_RESULT_FIELDS = _IDENTITY_FIELDS | {
    "operations_expected", "operations_observed", "test_verdict",
    "measurement_validity", "observations", "samples", "environment",
}
_PARAMETER_FIELDS = {"iterations", "repetitions", "batch_size", "measurement_requested"}
_SAMPLE_FIELDS = {"metric", "value", "unit", "repetition", "population"}


def _object(value, label):
    if type(value) is not dict:
        raise ValueError(f"{label} must be an object")
    return value


def _fields(value, required, label):
    _object(value, label)
    if set(value) != required:
        raise ValueError(f"{label} has missing or unknown fields")


def _text(value, label):
    if type(value) is not str or not value.strip() or "\x00" in value:
        raise ValueError(f"{label} must be nonempty text without NUL")


def _choice(value, choices, label):
    if type(value) is not str or value not in choices:
        raise ValueError(f"unsupported {label}")


def _integer(value, minimum, maximum, label):
    if type(value) is not int or not minimum <= value <= maximum:
        raise ValueError(f"{label} must be an integer in [{minimum}, {maximum}]")


def _identifiers(run_id, request_id, plan_sha256):
    _text(run_id, "run_id")
    _text(request_id, "request_id")
    if (
        type(plan_sha256) is not str or len(plan_sha256) != 64
        or any(char not in "0123456789abcdef" for char in plan_sha256)
    ):
        raise ValueError("plan_sha256 must be a lowercase SHA-256 hex digest")


def _target_text(value, label):
    if type(value) is not str or not value or len(value) > 4096:
        raise ValueError(f"{label} must be nonempty text of at most 4096 characters")
    if any(ord(character) < 0x20 or 0x7f <= ord(character) <= 0x9f for character in value):
        raise ValueError(f"{label} cannot contain C0, DEL or C1 controls")
    try:
        value.encode("utf-8")
    except UnicodeEncodeError as error:
        raise ValueError(f"{label} must contain valid Unicode scalar values") from error


def _target_pair(vendor, renderer, profile):
    if vendor is None and renderer is None:
        return None
    if vendor is None or renderer is None:
        raise ValueError("expected GL vendor and renderer must be supplied together")
    _target_text(vendor, "expected_gl_vendor")
    _target_text(renderer, "expected_gl_renderer")
    if profile != "opengl-hardware":
        raise ValueError("an expected GL target is admissible only for opengl-hardware")
    return vendor, renderer


def _hardware_environment(environment, target):
    vendor, renderer = target
    for key, expected in (
        ("expected_gl_vendor", vendor), ("expected_gl_renderer", renderer),
        ("gl_vendor", vendor), ("gl_renderer", renderer),
    ):
        observed = environment.get(key)
        _target_text(observed, key)
        if observed != expected:
            raise ValueError(f"{key} disagrees with the exact plan target")
    lowered = renderer.casefold()
    if any(name in lowered for name in (
        "llvmpipe", "softpipe", "swiftshader", "software", "swrast", "gdi generic",
    )):
        raise ValueError("a matching software renderer is not a hardware qualification")
    _choice(environment.get("qualification"), ("MATCHED_EXPECTATION",), "target qualification")
    if type(environment.get("context_current")) is not bool or not environment["context_current"]:
        raise ValueError("hardware evidence requires the context current on its owner thread")
    major, minor = environment.get("context_major"), environment.get("context_minor")
    _integer(major, 0, 2147483647, "context major version")
    _integer(minor, 0, 2147483647, "context minor version")
    mask = environment.get("context_profile_mask")
    _integer(mask, 0, 2147483647, "context profile mask")
    if (major, minor) < (3, 3) or mask != 1:
        raise ValueError("hardware evidence requires an unambiguous OpenGL 3.3+ core context")


def _selection(scenario, route, profile, parameters):
    _fields(parameters, _PARAMETER_FIELDS, "parameters")
    _integer(parameters["iterations"], 1, 1000000, "iterations")
    _integer(parameters["repetitions"], 1, 100, "repetitions")
    batch = parameters["batch_size"]
    if type(batch) is not int or batch not in (1, 16, 256):
        raise ValueError("batch_size must be 1, 16 or 256")
    if parameters["iterations"] * parameters["repetitions"] * batch > 100000000:
        raise ValueError("workload operation budget exceeded")
    if type(parameters["measurement_requested"]) is not bool:
        raise ValueError("measurement_requested must be bool")
    _choice(scenario, (
        "cpu.float-mirror", "gl.float-routes-correctness", "gl.float-submission-cpu",
    ), "workload scenario")
    if scenario == "cpu.float-mirror":
        _choice(profile, ("cpu-only",), "CPU profile")
        _choice(route, ("pass-name",), "CPU route")
        if parameters["measurement_requested"]:
            raise ValueError("CPU mirror does not produce submission measurements")
    else:
        _choice(profile, ("opengl-software", "opengl-hardware"), "GL profile")
        _choice(route, ("pass-name", "prepared-float"), "GL route")
        if scenario != "gl.float-submission-cpu" and parameters["measurement_requested"]:
            raise ValueError("correctness workload does not produce submission measurements")


def _identity(value):
    _fields(value, _IDENTITY_FIELDS, "expected identity")
    _integer(value["schema_version"], 1, 1, "schema_version")
    _integer(value["scenario_version"], 1, 1, "scenario_version")
    _identifiers(value["run_id"], value["request_id"], value["plan_sha256"])
    _selection(value["scenario"], value["route"], value["profile"], value["parameters"])


def build_workload_argv(
    *,
    executable: str,
    scenario: str,
    route: str,
    profile: str,
    iterations: int,
    repetitions: int,
    batch_size: int,
    measurement_requested: bool,
    request_id: str,
    run_id: str,
    plan_sha256: str,
    result_path: str,
    expected_gl_vendor: str | None = None,
    expected_gl_renderer: str | None = None,
) -> tuple[str, ...]:
    """Validate the finite invocation and optional exact target without effects."""
    _text(executable, "executable")
    _text(result_path, "result_path")
    _identifiers(run_id, request_id, plan_sha256)
    _selection(scenario, route, profile, {
        "iterations": iterations, "repetitions": repetitions,
        "batch_size": batch_size, "measurement_requested": measurement_requested,
    })
    target = _target_pair(expected_gl_vendor, expected_gl_renderer, profile)
    argv = (
        executable,
        "--scenario", scenario,
        "--route", route,
        "--iterations", str(iterations),
        "--repetitions", str(repetitions),
        "--batch-size", str(batch_size),
        "--request-id", request_id,
        "--run-id", run_id,
        "--plan-sha256", plan_sha256,
        "--result", result_path,
    )
    if scenario != "cpu.float-mirror":
        argv += (
            "--profile", profile,
            "--measurement-requested", str(measurement_requested).lower(),
        )
        if target is not None:
            argv += (
                "--expected-gl-vendor", target[0],
                "--expected-gl-renderer", target[1],
            )
    return argv


def _unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def _reject_constant(value):
    raise ValueError(f"nonfinite JSON constant: {value}")


def _operations(document):
    parameters = document["parameters"]
    total = parameters["iterations"] * parameters["repetitions"] * parameters["batch_size"]
    expected = _object(document["operations_expected"], "operations_expected")
    observed = _object(document["operations_observed"], "operations_observed")
    if not {"updates", "draws"} <= set(expected) or set(observed) != set(expected):
        raise ValueError("operation names are missing or inconsistent")
    for counters in (expected, observed):
        for name, count in counters.items():
            _text(name, "operation name")
            _integer(count, 0, 100000000, f"operation {name}")
    draws = 0 if document["scenario"] == "cpu.float-mirror" else total
    if expected["updates"] != total or expected["draws"] != draws:
        raise ValueError("declared operation counts disagree with workload parameters")
    if document["test_verdict"] == "PASS" and observed != expected:
        raise ValueError("PASS requires all declared operations")


def _samples(document):
    samples = document["samples"]
    if type(samples) is not list:
        raise ValueError("samples must be an array")
    validity = document["measurement_validity"]
    requested = document["parameters"]["measurement_requested"]
    if not requested:
        if validity != "NOT_REQUESTED" or samples:
            raise ValueError("unrequested measurement must have no samples")
        return
    if validity == "NOT_REQUESTED":
        raise ValueError("requested measurement cannot be NOT_REQUESTED")
    if validity == "ABSENT":
        if samples:
            raise ValueError("ABSENT measurement cannot contain samples")
        return
    if validity == "VALID" and document["test_verdict"] != "PASS":
        raise ValueError("only a passing oracle may have VALID measurements")
    parameters = document["parameters"]
    population = parameters["iterations"] * parameters["batch_size"]
    repetitions = parameters["repetitions"]
    seen = set()
    for sample in samples:
        _fields(sample, _SAMPLE_FIELDS, "sample")
        if sample["metric"] != "cpu_wall_submission" or sample["unit"] != "ns":
            raise ValueError("sample must describe CPU wall submission time in ns")
        _integer(sample["repetition"], 0, repetitions - 1, "sample repetition")
        _integer(sample["population"], population, population, "sample population")
        if sample["repetition"] in seen:
            raise ValueError("duplicate sample repetition")
        seen.add(sample["repetition"])
        value = sample["value"]
        if type(value) not in (int, float):
            raise ValueError("sample value must be numeric, not bool")
        try:
            finite = math.isfinite(value)
        except OverflowError:
            finite = False
        if not finite or value < 0 or (validity == "VALID" and value == 0):
            raise ValueError("sample elapsed time is invalid")
    if validity == "VALID" and seen != set(range(repetitions)):
        raise ValueError("VALID measurement requires exactly one sample per repetition")


def parse_workload_result(text: str, *, expected: dict) -> dict:
    """Reconcile JSON with the invocation, preserving explicit domain failures.

    Identity is checked even for FAIL. ABSENT is preserved, never converted to a
    zero sample. File attribution, renderer qualification and aggregation remain
    responsibilities of the collector; this function only processes memory.
    """
    if type(text) is not str:
        raise ValueError("result must be JSON text")
    document = json.loads(text, object_pairs_hook=_unique_object, parse_constant=_reject_constant)
    _fields(document, _RESULT_FIELDS, "result")
    _object(expected, "expected identity")
    # Optional plan expectations are not new result identity/parameters fields.
    identity_expectation = {
        key: value for key, value in expected.items()
        if key not in ("expected_gl_vendor", "expected_gl_renderer")
    }
    _identity(identity_expectation)
    target = _target_pair(
        expected.get("expected_gl_vendor"), expected.get("expected_gl_renderer"),
        identity_expectation["profile"],
    )
    actual_identity = {key: document[key] for key in _IDENTITY_FIELDS}
    _identity(actual_identity)
    if actual_identity != identity_expectation:
        raise ValueError("result identity or parameters disagree with invocation")
    _choice(document["test_verdict"], ("NOT_EXECUTED", "PASS", "FAIL", "INCONCLUSIVE"), "test verdict")
    _choice(document["measurement_validity"], ("NOT_REQUESTED", "ABSENT", "VALID", "INVALID"), "measurement validity")
    _object(document["observations"], "observations")
    _object(document["environment"], "environment")
    if target is not None:
        # This also runs for FAIL; domain failure must not conceal foreign or
        # insufficient observations. It does not claim universal physical
        # acceleration or advanced capabilities from matching device strings.
        _hardware_environment(document["environment"], target)
    _operations(document)
    _samples(document)
    return document
