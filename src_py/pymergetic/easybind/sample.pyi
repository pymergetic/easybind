from __future__ import annotations

from enum import Enum

DEFAULT_VALUE: int
PI: float
LIB_NAME: str


class SampleError(RuntimeError): ...


class WidgetError(SampleError): ...


class WidgetKind(Enum):
    Basic: WidgetKind
    Fancy: WidgetKind
    Deluxe: WidgetKind


class Widget:
    name: str
    value: int
    kind: WidgetKind

    def __init__(self, name: str = ..., value: int = ..., kind: WidgetKind = ...) -> None: ...
    def bump(self, delta: int = ...) -> int: ...
    def summary(self) -> str: ...
    def kind_name(self) -> str: ...


def add(left: int, right: int) -> int: ...
def greet(name: str) -> str: ...
def make_widget(name: str, value: int = ..., kind: WidgetKind = ...) -> Widget: ...
def raise_sample_error(message: str) -> None: ...
def raise_widget_error(message: str) -> None: ...
