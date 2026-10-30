"""Pure structural boundary shared by the separately owned platform adapters."""
from typing import Protocol

from ..model import (
    BuildReceipt, CommandSpec, PreparationReceipt, VerificationPlan,
)


class PlatformAdapter(Protocol):
    def describe_preparation(self, plan: VerificationPlan, *,
                             bundle_root: str) -> tuple[CommandSpec, ...]:
        ...

    def describe_build(self, plan: VerificationPlan,
                       preparation: PreparationReceipt) -> tuple[CommandSpec, ...]:
        ...

    def describe_execution(self, plan: VerificationPlan,
                           build: BuildReceipt) -> tuple[CommandSpec, ...]:
        ...

    def collect_environment(self) -> dict:
        ...
