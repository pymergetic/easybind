from __future__ import annotations

from enum import Enum
from typing import Awaitable, Any

class WidgetKind(Enum):
    Basic: WidgetKind
    Fancy: WidgetKind

class Widget:
    name: str
    value: int
    kind: WidgetKind

    def __init__(self) -> None: ...
    def summary(self) -> Awaitable[str]: ...
    def bump(self, delta: int = 1) -> Awaitable[int]: ...

def make_widget(name: str, value: int = 0, kind: WidgetKind = WidgetKind.Basic) -> Widget: ...
def async_add(a: int, b: int) -> Any: ...
