"""Resolve only closed catalogue metadata; perform no local observations."""
from .model import Request, ScenarioSpec, VerificationPlan


def make_plan(request: Request, catalogue: tuple[ScenarioSpec, ...]) -> VerificationPlan:
    if not isinstance(request, Request):
        raise TypeError('request must be a Request')
    if not isinstance(catalogue, tuple) or any(not isinstance(spec, ScenarioSpec) for spec in catalogue):
        raise TypeError('catalogue must be a tuple of ScenarioSpec records')
    by_id = {spec.id: spec for spec in catalogue}
    if len(by_id) != len(catalogue):
        raise ValueError('catalogue contains duplicate scenario ids')
    if any(name not in by_id for name in request.scenarios):
        raise ValueError('scenario selection is not in the closed catalogue')
    selected = tuple(by_id[name] for name in request.scenarios)
    unresolved = ('source_identity', 'toolchain', 'executables')
    if request.profile != 'cpu-only':
        unresolved += ('gl_context', 'renderer_identity')
    if request.profile == 'opengl-hardware' and request.expected_gl_vendor is None:
        unresolved += ('expected_gl_target',)
    # The adapter describes concrete commands only in the explicit effectful
    # phase. A consultative plan never probes a build/device or loads an adapter.
    return VerificationPlan(1, request, selected, (), unresolved)
