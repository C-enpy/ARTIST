"""Private, literal environment-to-CLI transport for the manual workflow.

Admission, identities, predecessor receipts and evidence remain owned by verify.
Importing this module does not invoke the CLI or inspect the machine.
"""
import sys

sys.dont_write_bytecode = True


def build_argv(env):
    platform = env.get('VERIFY_PLATFORM', '')
    profile = env.get('VERIFY_PROFILE', '')
    phase = env.get('VERIFY_PHASE', '')
    configuration = env.get('VERIFY_CONFIGURATION', '')
    scenario = env.get('VERIFY_SCENARIO', '')
    allow = env.get('VERIFY_ALLOW_DEPENDENCY_RESOLUTION', 'false')
    vendor = env.get('VERIFY_EXPECTED_GL_VENDOR', '')
    renderer = env.get('VERIFY_EXPECTED_GL_RENDERER', '')
    bundle = env.get('VERIFY_BUNDLE', '')
    output_root = env.get('VERIFY_OUTPUT_ROOT', 'out/verification')
    for value in (platform, profile, phase, configuration, scenario, allow,
                  vendor, renderer, bundle, output_root):
        if not isinstance(value, str):
            raise ValueError('workflow inputs must be strings')
        if any(ord(char) < 32 or 127 <= ord(char) <= 159
               or 0xD800 <= ord(char) <= 0xDFFF for char in value):
            raise ValueError('workflow inputs must not contain controls or surrogates')
    if platform not in ('windows', 'linux'):
        raise ValueError('unsupported workflow platform')
    if profile not in ('cpu-only', 'opengl-software', 'opengl-hardware'):
        raise ValueError('unsupported workflow profile')
    if platform == 'windows' and profile == 'opengl-software':
        raise ValueError('Windows software OpenGL is not supported')
    if phase not in ('list', 'plan', 'prepare', 'build', 'run', 'report'):
        raise ValueError('unsupported workflow phase')
    if configuration not in ('debug', 'release'):
        raise ValueError('unsupported workflow configuration')
    if scenario not in ('cpu.float-mirror', 'gl.float-routes-correctness',
                        'gl.float-preparation', 'gl.float-lifetime', 'gl.float-submission-cpu'):
        raise ValueError('unsupported workflow scenario')
    if allow not in ('true', 'false'):
        raise ValueError('dependency resolution must be true or false')
    if allow == 'true' and phase != 'prepare':
        raise ValueError('dependency resolution opt-in is restricted to prepare')
    if profile == 'opengl-hardware':
        if not vendor.strip() or not renderer.strip():
            raise ValueError('hardware requires exact nonempty vendor and renderer')
    elif vendor or renderer:
        raise ValueError('hardware targets require the hardware profile')
    if output_root != 'out/verification':
        raise ValueError('workflow output root must be out/verification')
    if phase == 'list':
        return ('list', '--json')
    if phase in ('build', 'run', 'report'):
        if not bundle.strip():
            raise ValueError('this phase requires an existing bundle')
        # No filesystem check, repair, discovery or replacement here. The core
        # admits the exact predecessor, plan, source root and candidate identity.
        return (phase, '--bundle', bundle) + (('--json',) if phase == 'report' else ())
    argv = (phase, '--platform', platform, '--profile', profile,
            '--scenario', scenario, '--configuration', configuration)
    if profile == 'opengl-hardware':
        argv += ('--expected-gl-vendor', vendor, '--expected-gl-renderer', renderer)
    if phase == 'prepare':
        if allow != 'true':
            raise ValueError('prepare requires explicit dependency resolution opt-in')
        argv += ('--output-root', output_root, '--allow-dependency-resolution')
    return argv


def dispatch(env, *, invoke):
    return invoke(build_argv(env))


if __name__ == '__main__':
    import os
    from verify import main

    try:
        raise SystemExit(dispatch(os.environ, invoke=main))
    except ValueError as error:
        print('Invalid workflow request: ' + str(error), file=sys.stderr)
        raise SystemExit(2)
