"""Ratified legacy-context transport: inherited state is never opt-in.

Pure Popen double only. No context parser/producer, child or GL is imported/run.
"""
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from verification.model import CommandSpec
from verification.runner import SubprocessRunner


PUBLIC_CONTEXT = {
    'ARTIST_VERIFICATION_CONTEXT_V1': 'opengl-hardware',
    'ARTIST_VERIFICATION_RUN_ID': 'declared-run',
    'ARTIST_VERIFICATION_PLAN_SHA256': 'a' * 64,
    'ARTIST_VERIFICATION_REQUEST_ID': 'declared-invocation',
    'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': ' Exact Vendor ',
    'ARTIST_VERIFICATION_EXPECTED_GL_RENDERER': '\u00a0Exact Renderer ',
}


class ContextTransportContractTests(unittest.TestCase):
    def test_inherited_public_context_is_removed_before_only_declared_delta_is_applied(self):
        inherited = dict(PUBLIC_CONTEXT,
                         ARTIST_VERIFICATION_RUN_ID='ambient-run',
                         ARTIST_VERIFICATION_REQUEST_ID='ambient-invocation',
                         ARTIST_TEST_ORDINARY_ENV='ordinary-parent-value')
        declarations = ({},
                        {'ARTIST_VERIFICATION_EXPECTED_GL_VENDOR': '\u00a0'},
                        dict(PUBLIC_CONTEXT))
        with tempfile.TemporaryDirectory(prefix='owned context transport ') as directory:
            for declaration in declarations:
                with self.subTest(declared_keys=sorted(declaration)), patch.dict(os.environ, inherited):
                    parent = {key: os.environ.get(key) for key in inherited}
                    environments = []

                    class OwnedProcess:
                        returncode = 0

                        def communicate(self, *args, **kwargs):
                            return b'owned context transport', b''

                        def wait(self, timeout=None):
                            return self.returncode

                        def poll(self):
                            return 0

                    def factory(argv, **kwargs):
                        self.assertEqual(tuple(argv), ('owned-context-transport-double',))
                        self.assertFalse(kwargs.get('shell', False))
                        environments.append(dict(kwargs['env']))
                        return OwnedProcess()

                    command = CommandSpec(('owned-context-transport-double',), directory,
                                          declaration, 1, 'run', ('owned-process-double',))
                    outcome = SubprocessRunner(popen_factory=factory).run(command)
                    self.assertEqual(outcome.returncode, 0)
                    self.assertEqual(len(environments), 1)
                    child = environments[0]
                    actual = {key: child[key] for key in PUBLIC_CONTEXT if key in child}
                    self.assertEqual(actual, declaration,
                                     'ambient context identities must not activate or complete an explicit request')
                    self.assertEqual(child['ARTIST_TEST_ORDINARY_ENV'], 'ordinary-parent-value')
                    self.assertEqual({key: os.environ.get(key) for key in inherited}, parent)


if __name__ == '__main__':
    unittest.main()
