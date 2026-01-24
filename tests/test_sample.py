import asyncio

from pymergetic.easybind.sample import _sample as sample  # type: ignore[import-not-found]


def test_widget_fields_and_enum() -> None:
    widget = sample.Widget()
    widget.name = "alpha"
    widget.value = 3
    widget.kind = sample.WidgetKind.Fancy

    assert widget.name == "alpha"
    assert widget.value == 3
    assert widget.kind is sample.WidgetKind.Fancy


def test_make_widget() -> None:
    widget = sample.make_widget("beta", value=5, kind=sample.WidgetKind.Basic)
    assert widget.name == "beta"
    assert widget.value == 5
    assert widget.kind is sample.WidgetKind.Basic


def test_async_methods_and_function() -> None:
    async def run() -> None:
        widget = sample.Widget()
        widget.name = "gamma"
        widget.value = 7
        widget.kind = sample.WidgetKind.Basic

        assert await widget.summary() == "gamma:7:basic"
        assert await widget.bump(4) == 11
        assert await sample.async_add(2, 9) == 11

    asyncio.run(run())
