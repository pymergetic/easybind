import enum
from typing import overload


class WidgetKind(enum.Enum):
    Basic = 0

    Fancy = 1

    Deluxe = 2

Basic: WidgetKind_easy = WidgetKind_easy.Basic

Fancy: WidgetKind_easy = WidgetKind_easy.Fancy

Deluxe: WidgetKind_easy = WidgetKind_easy.Deluxe

class WidgetKind_easy(enum.Enum):
    Basic = 0

    Fancy = 1

    Deluxe = 2

class SampleError_easy(Exception):
    pass

class WidgetError_easy(SampleError_easy):
    pass

kDefaultValue_easy: int = 42

kPi_easy: float = 3.141592653589793

kLibName_easy: str = 'pymergetic.easybind.sample'

def add_easy(left: int, right: int) -> int: ...

def greet_easy(name: str) -> str: ...

def make_widget_easy(name: str, value: int = 0, kind: WidgetKind = WidgetKind.Basic) -> Widget: ...

class SampleError(Exception):
    pass

class WidgetError(SampleError):
    pass

class Widget:
    @overload
    def __init__(self) -> None: ...

    @overload
    def __init__(self, name: str, value: int = 0, kind: WidgetKind = WidgetKind.Basic) -> None: ...

    @property
    def name(self) -> str: ...

    @name.setter
    def name(self, arg: str, /) -> None: ...

    @property
    def value(self) -> int: ...

    @value.setter
    def value(self, arg: int, /) -> None: ...

    @property
    def kind(self) -> WidgetKind: ...

    @kind.setter
    def kind(self, arg: WidgetKind, /) -> None: ...

    def bump(self, delta: int = 1) -> int: ...

    def summary(self) -> str: ...

    def kind_name(self) -> str: ...

def raise_sample_error(message: str) -> None: ...

def raise_widget_error(message: str) -> None: ...

kDefaultValue: int = 42

kPi: float = 3.141592653589793

kLibName: str = 'pymergetic.easybind.sample'

def add(left: int, right: int) -> int: ...

def greet(name: str) -> str: ...

def make_widget(name: str, value: int = 0, kind: WidgetKind = WidgetKind.Basic) -> Widget: ...
