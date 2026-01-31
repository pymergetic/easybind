import enum


class FlagState(enum.Enum):
    Unknown = 0

    False = 1

    True = 2

class ModuleNode:
    @property
    def name(self) -> str: ...

    def full_name(self) -> str: ...

    @property
    def parent(self) -> ModuleNode: ...

    @property
    def package_state(self) -> FlagState: ...

    @property
    def package(self) -> bool: ...

    @property
    def shared_object_state(self) -> FlagState: ...

    @property
    def shared_object(self) -> bool: ...

    def children(self) -> list[ModuleNode]: ...

def root() -> ModuleNode: ...

def from_full_name(full_name: str) -> ModuleNode: ...

def create(full_name: str, is_package: bool = False, shared_object: bool = False) -> ModuleNode: ...
