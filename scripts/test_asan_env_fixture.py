"""Fixture-level regression contract for the H-RESTORE staging helpers (R-A2 P2).

Scope: scripts/test_asan_env.py staging/release helpers and the
check_wrapper_restoration entry, exercised on a private replica layout root
under a tempfile.TemporaryDirectory. No real build, no PowerShell wrapper arm,
no GPU, no Conan, no write to the principal repository: every file these tests
touch lives inside the private temporary directory (the runner's evidence
subtree out/asan-caller-restoration/<label> is created inside the private
repo root as well).

Fault injection uses unittest.mock on pathlib.Path methods. That proves the
Python error path of the runner; it does not prove a real OS failure, a real
disk condition or GPU behavior.

Causal RED contract (first failures preserved under out/h-fixture/):
a staging failure that happens AFTER the real build directory was moved
aside must (1) restore that directory with its artifacts intact,
(2) leave no .hold-hrestore or empty fixture artifact behind, (3) keep the
initial error observable, and (4) never delete foreign or non-empty
artifacts to force a pass. A rollback that itself fails must be reported
explicitly together with the original error.

Run from the repository root (the four real PowerShell arms are never started:
subprocess is asserted not called):
    python -m unittest discover -s scripts -p test_asan_env_fixture.py -v
"""

import hashlib
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

try:
    import test_asan_env as runner_mod
except ModuleNotFoundError:  # direct execution: python scripts/test_asan_env_fixture.py
    sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
    import test_asan_env as runner_mod

_runner = runner_mod

WITNESS_BYTES = b"real-build-dir witness for the H-RESTORE fixture contract\n"
NESTED_BYTES = b"nested generated artifact witness\n"
FOREIGN_BYTES = b"foreign artifact content (never delete or overwrite me)\n"
STALE_HOLD_BYTES = b"stale hold leftover content\n"
SENTINEL_PS_BYTES = b"sentinel ps1 (never executed by the fixture tests)\n"


def _sha256_bytes(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _sha256_bytes_type(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


class BuildDirFixture:
    """Private replica of the runner layout used by check_wrapper_restoration:
    repo/scripts (sentinel ps1 files) plus repo/out/build/windows-msvc-debug-asan."""

    def __init__(self, temp_root: pathlib.Path, kind: str) -> None:
        repo_root = temp_root / "repo"
        scripts_dir = repo_root / "scripts"
        scripts_dir.mkdir(parents=True)
        (scripts_dir / "build-windows.ps1").write_bytes(SENTINEL_PS_BYTES)
        (scripts_dir / "asan-env.ps1").write_bytes(SENTINEL_PS_BYTES)
        self.repo_root = repo_root
        self.scripts_dir = scripts_dir
        # Same composition as check_wrapper_restoration: repo_root = scripts_dir.parent.
        self.build_dir = repo_root / "out" / "build" / "windows-msvc-debug-asan"
        self.build_dir.parent.mkdir(parents=True, exist_ok=True)
        if kind == "real-dir":
            self.build_dir.mkdir(parents=True)
            (self.build_dir / "asan-witness.txt").write_bytes(WITNESS_BYTES)
            nested = self.build_dir / "generated"
            nested.mkdir()
            (nested / "nested.bin").write_bytes(NESTED_BYTES)
        elif kind == "empty-file":
            self.build_dir.write_bytes(b"")
        elif kind == "foreign-file":
            self.build_dir.write_bytes(FOREIGN_BYTES)
        elif kind == "absent":
            pass
        else:
            raise ValueError(f"unknown fixture kind: {kind}")
        self.hold = _runner._hold_path(self.build_dir)

    def witness_hash(self) -> str:
        return _sha256_bytes(self.build_dir / "asan-witness.txt")

    def nested_hash(self) -> str:
        return _sha256_bytes(self.build_dir / "generated" / "nested.bin")

    def state_summary(self) -> str:
        return (
            f"post-state: build_dir exists={self.build_dir.exists()} "
            f"is_dir={self.build_dir.is_dir()} is_file={self.build_dir.is_file()} "
            f"hold_exists={self.hold.exists()} hold_is_dir={self.hold.is_dir()}"
        )


def _patch_path_write_bytes(target: pathlib.Path, on_target) -> mock.patch:
    """Restore-mode patch: only writes going to `target` reach `on_target`;
    every other call is delegated to the original implementation."""
    original = pathlib.Path.write_bytes

    def fake(self_path: pathlib.Path, data: bytes):
        if self_path == target:
            return on_target(original, self_path, data)
        return original(self_path, data)

    return mock.patch.object(pathlib.Path, "write_bytes", fake)


def _patch_path_rename(raise_on) -> mock.patch:
    """Runs the injection callback for every Path.rename; when it returns an
    exception instance, that exception is raised instead of performing the
    rename (other renames are delegated to the original implementation)."""
    original = pathlib.Path.rename

    def fake(self_path: pathlib.Path, target: pathlib.Path):
        injection = raise_on(original, self_path, target)
        if injection is not None:
            raise injection
        return original(self_path, target)

    return mock.patch.object(pathlib.Path, "rename", fake)


def _no_powershell_patches():
    """The staging fault happens before any arm; prove the runner never starts
    a wrapper arm and never spawns any process at all (no PowerShell).
    Returns (patches to enter around the call, subprocess mock, arm mock)."""
    subprocess_mock = mock.MagicMock(name="subprocess_replacement")
    arm_mock = mock.MagicMock(name="run_wrapper_restoration_arm_replacement")
    patches = (
        mock.patch.object(_runner, "subprocess", new=subprocess_mock),
        mock.patch.object(_runner, "run_wrapper_restoration_arm", new=arm_mock),
    )
    return patches, subprocess_mock, arm_mock


def _raise_blank_renamed_without_file(original, path: pathlib.Path, data: bytes) -> None:
    raise OSError(22, "injected staging write failure at the fixture location (unittest.mock fault)")


def _raise_after_partial_empty_file(original, path: pathlib.Path, data: bytes) -> None:
    original(path, data)  # the failed write attempt still created the empty fixture file
    raise OSError(22, "injected staging write failure after a partially created empty file")


def _raise_after_foreign_partial_file(original, path: pathlib.Path, data: bytes) -> None:
    original(path, FOREIGN_BYTES)  # partial write left non-empty (foreign-looking) content
    raise OSError(22, "injected staging write failure after a foreign-looking partial write")


class WriteErrorAfterRenameRestoreTests(unittest.TestCase):
    """Causal RED: staging fixture failure after the real directory was moved
    aside must restore it; no hold/fixture may survive; error stays observable;
    no PowerShell wrapper arm is ever started."""

    def _run_with_write_fault(self, fixture: BuildDirFixture, on_target, label: str):
        patches, subprocess_mock, arm_mock = _no_powershell_patches()
        with _patch_path_write_bytes(fixture.build_dir, on_target), patches[0], patches[1], \
                self.assertRaises(OSError) as raised:
            _runner.check_wrapper_restoration(fixture.scripts_dir, label)
        return raised.exception, subprocess_mock, arm_mock

    def _assert_original_artifacts_restored(self, fixture: BuildDirFixture) -> None:
        self.assertTrue(
            fixture.build_dir.is_dir(),
            "original build directory must be restored as a directory; " + fixture.state_summary(),
        )
        witness = fixture.build_dir / "asan-witness.txt"
        self.assertTrue(witness.is_file(), "witness must survive the staging failure; " + fixture.state_summary())
        self.assertEqual(
            fixture.witness_hash(), _sha256_bytes_type(WITNESS_BYTES),
            "witness content must be byte-for-byte intact after restoration",
        )
        self.assertEqual(
            fixture.nested_hash(), _sha256_bytes_type(NESTED_BYTES),
            "nested artifact content must be byte-for-byte intact after restoration",
        )
        self.assertFalse(
            fixture.hold.exists(),
            "no .hold-hrestore may survive a staging failure; " + fixture.state_summary(),
        )
        self.assertFalse(
            fixture.build_dir.is_file(),
            "no empty fixture file may survive at the original path; " + fixture.state_summary(),
        )

    def _assert_no_powershell(self, subprocess_mock, arm_mock) -> None:
        subprocess_mock.run.assert_not_called()
        arm_mock.assert_not_called()

    def test_write_error_after_rename_restores_original_build_dir(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "real-dir")
            error, subprocess_mock, arm_mock = self._run_with_write_fault(
                fixture, _raise_blank_renamed_without_file, "red-write-error-after-rename"
            )
            self.assertIn("injected staging write failure", str(error))
            self._assert_original_artifacts_restored(fixture)
            self._assert_no_powershell(subprocess_mock, arm_mock)

    def test_write_error_after_partial_empty_file_creation_restores_original_build_dir(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "real-dir")
            error, subprocess_mock, arm_mock = self._run_with_write_fault(
                fixture, _raise_after_partial_empty_file, "red-write-error-partial-empty-file"
            )
            self.assertIn("injected staging write failure", str(error))
            self._assert_original_artifacts_restored(fixture)
            self._assert_no_powershell(subprocess_mock, arm_mock)


class RollbackFailureReportingTests(unittest.TestCase):
    """A rollback that itself fails must be reported explicitly together with
    the original error, and pre-existing/foreign non-empty artifacts must never
    be removed or overwritten to force a pass."""

    def test_restore_rename_failure_reports_both_errors_explicitly(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "real-dir")
            witness_before = fixture.witness_hash()

            def refuse_rollback_rename(original, path, target):
                if path == fixture.hold and target == fixture.build_dir:
                    return PermissionError(13, "injected restoration rename failure")
                return None

            subprocess_patches, subprocess_mock, arm_mock = _no_powershell_patches()
            with _patch_path_write_bytes(fixture.build_dir, _raise_blank_renamed_without_file), \
                    _patch_path_rename(refuse_rollback_rename), subprocess_patches[0], subprocess_patches[1], \
                    self.assertRaises(RuntimeError) as raised:
                _runner.check_wrapper_restoration(fixture.scripts_dir, "etape-restoration-rename-failure")

            error = raised.exception
            self.assertIn("original error", str(error), "the initial error must stay observable")
            self.assertIn("restoration error", str(error), "the rollback failure must be reported explicitly")
            self.assertIn("injected staging write failure", str(error))
            self.assertIn("injected restoration rename failure", str(error))
            self.assertIn("could not move", str(error))
            self.assertIsInstance(error.__cause__, OSError, "original error must be chained")
            # Explicitly failing rollback: artifacts remain displaced AT the hold,
            # rather than being claimed restored or silently lost.
            self.assertTrue(
                fixture.hold.is_dir(),
                "when rollback fails the artifacts must remain identifiable at the hold; " + fixture.state_summary(),
            )
            self.assertEqual(_sha256_bytes(fixture.hold / "asan-witness.txt"), witness_before)
            self.assertFalse(
                (fixture.build_dir / "asan-witness.txt").exists(),
                "restoration did not happen: nothing may claim the original path; " + fixture.state_summary(),
            )
            subprocess_mock.run.assert_not_called()
            arm_mock.assert_not_called()

    def test_foreign_non_empty_file_during_failed_write_is_never_deleted(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "real-dir")
            witness_before = fixture.witness_hash()

            subprocess_patches, subprocess_mock, arm_mock = _no_powershell_patches()
            with _patch_path_write_bytes(fixture.build_dir, _raise_after_foreign_partial_file), \
                    subprocess_patches[0], subprocess_patches[1], \
                    self.assertRaises(RuntimeError) as raised:
                _runner.check_wrapper_restoration(fixture.scripts_dir, "etape-foreign-partial-file")

            error = raised.exception
            self.assertIn("refusing", str(error))
            self.assertIn("original error", str(error), "the initial error must stay observable")
            self.assertIn("restoration error", str(error), "the rollback refusal must be reported explicitly")
            self.assertIsInstance(error.__cause__, OSError, "original error must be chained")
            # The foreign non-empty artifact is preserved byte-for-byte, never
            # deleted or overwritten by the rollback.
            self.assertTrue(
                fixture.build_dir.is_file(),
                "the artifact at the original path must survive the rollback refusal; " + fixture.state_summary(),
            )
            self.assertEqual(_sha256_bytes(fixture.build_dir), _sha256_bytes_type(FOREIGN_BYTES))
            # Restoration was impossible without deleting a foreign artifact: the
            # real directory remains displaced at the hold, explicitly reported.
            self.assertTrue(fixture.hold.is_dir(), fixture.state_summary())
            self.assertEqual(_sha256_bytes(fixture.hold / "asan-witness.txt"), witness_before)
            subprocess_mock.run.assert_not_called()
            arm_mock.assert_not_called()


class StagingAndReleaseNominalContractTests(unittest.TestCase):
    """Nominal staging/release behavior and refuse-without-mutation contracts.
    Characterization of documented behavior: expected identical before and
    after the R-A2 rollback fix."""

    def test_stage_and_release_roundtrip_with_preexisting_directory(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "real-dir")
            staged = _runner._stage_build_dir_file(fixture.build_dir)
            self.assertEqual(staged, fixture.hold)
            self.assertTrue(staged.is_dir(), "the hold must contain the displaced real directory")
            self.assertEqual(
                _sha256_bytes(staged / "asan-witness.txt"), _sha256_bytes_type(WITNESS_BYTES)
            )
            self.assertTrue(fixture.build_dir.is_file(), "staged location must be the empty fixture file")
            self.assertEqual(fixture.build_dir.stat().st_size, 0)
            _runner._release_build_dir_file(fixture.build_dir, staged)
            self.assertTrue(fixture.build_dir.is_dir(), "release must restore the real directory")
            self.assertEqual(fixture.witness_hash(), _sha256_bytes_type(WITNESS_BYTES))
            self.assertEqual(fixture.nested_hash(), _sha256_bytes_type(NESTED_BYTES))
            self.assertFalse(fixture.build_dir.is_file())
            self.assertFalse(fixture.hold.exists(), "release must remove the hold by moving it back")

    def test_stage_and_release_without_preexisting_directory(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "absent")
            staged = _runner._stage_build_dir_file(fixture.build_dir)
            self.assertIsNone(staged)
            self.assertTrue(fixture.build_dir.is_file())
            self.assertEqual(fixture.build_dir.stat().st_size, 0)
            self.assertFalse(fixture.hold.exists(), "no hold must be created when nothing was moved")
            _runner._release_build_dir_file(fixture.build_dir, None)
            self.assertFalse(fixture.build_dir.exists())
            self.assertFalse(fixture.hold.exists())

    def test_existing_hold_path_is_rejected_without_mutation(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "real-dir")
            witness_before = fixture.witness_hash()
            fixture.hold.write_bytes(STALE_HOLD_BYTES)
            with self.assertRaises(RuntimeError) as raised:
                _runner._stage_build_dir_file(fixture.build_dir)
            self.assertIn("hold path already exists", str(raised.exception))
            # Nothing mutated: the real directory and the stale hold are untouched.
            self.assertTrue(fixture.build_dir.is_dir())
            self.assertEqual(fixture.witness_hash(), witness_before)
            self.assertTrue(fixture.hold.is_file())
            self.assertEqual(fixture.hold.read_bytes(), STALE_HOLD_BYTES)

    def test_foreign_non_empty_file_is_rejected_without_mutation(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "foreign-file")
            failures: list[str] = []
            _runner._normalize_build_dir(fixture.build_dir, failures)
            self.assertEqual(len(failures), 1, "normalize must report the foreign file as a failure")
            self.assertIn("unexpected non-fixture file", failures[0])
            self.assertEqual(
                fixture.build_dir.read_bytes(), FOREIGN_BYTES,
                "normalize must NOT delete a foreign non-empty file",
            )
            with self.assertRaises(RuntimeError) as raised:
                _runner._stage_build_dir_file(fixture.build_dir)
            self.assertIn("unexpected non-empty file", str(raised.exception))
            self.assertEqual(fixture.build_dir.read_bytes(), FOREIGN_BYTES)
            self.assertFalse(fixture.hold.exists(), "no hold may be created for a foreign file")

    def test_normalize_reclaims_empty_fixture_file(self):
        with tempfile.TemporaryDirectory(prefix="h-fixture-") as tmp:
            fixture = BuildDirFixture(pathlib.Path(tmp), "empty-file")
            failures: list[str] = []
            _runner._normalize_build_dir(fixture.build_dir, failures)
            self.assertEqual(failures, [])
            self.assertFalse(
                fixture.build_dir.exists(),
                "an empty fixture file at the build directory is our own artifact and is reclaimed",
            )


if __name__ == "__main__":
    unittest.main()
