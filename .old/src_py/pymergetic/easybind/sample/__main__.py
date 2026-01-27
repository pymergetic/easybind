"""Run a small demo for easybind.sample."""

from __future__ import annotations

import asyncio

from pymergetic.easybind import __cpp__ as cpp_module  # type: ignore[import-not-found]
from pymergetic.easybind import __version__ as py_version

from . import SampleError, Widget, WidgetKind, async_add, make_widget, raise_error

async def _run_async() -> None:
    print(f"easybind python version: {py_version}")
    print(f"easybind __cpp__ version: {cpp_module.__version__}")

    w = Widget()
    w.name = "test"
    w.value = 10
    w.kind = WidgetKind.Fancy

    print(await w.summary())
    print(f"bump(+5) => {await w.bump(5)}")

    direct = make_widget("direct", value=3, kind=WidgetKind.Basic)
    print(await direct.summary())

    result = await async_add(2, 7)
    print(f"async_add(2, 7) => {result}")
    try:
        raise_error("boom")
    except SampleError as exc:
        print(f"caught SampleError: {exc}")

asyncio.run(_run_async())
