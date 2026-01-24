"""Run a small demo for easybind.sample."""

from __future__ import annotations

from . import _native

if _native is None:
    raise RuntimeError("pymergetic.easybind.sample native module is not available")

import asyncio

from ._sample import Widget, WidgetKind, async_add, make_widget  # type: ignore[import-not-found]

async def _run_async() -> None:
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

asyncio.run(_run_async())
