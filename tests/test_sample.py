from __future__ import annotations

import pytest

import easybind.sample as sample


def test_functions_and_constants() -> None:
    assert sample.add(2, 3) == 5
    assert sample.greet("easybind") == "hello easybind"
    assert sample.kDefaultValue == 42
    assert sample.kLibName == "easybind.sample"
    assert abs(sample.kPi - 3.141592653589793) < 1e-12
    assert sample.add_easy(2, 3) == 5
    assert sample.greet_easy("easybind") == "hello easybind"
    assert sample.kDefaultValue_easy == 42
    assert sample.kLibName_easy == "easybind.sample"
    assert abs(sample.kPi_easy - 3.141592653589793) < 1e-12


def test_widget() -> None:
    widget = sample.Widget("alpha", value=3, kind=sample.WidgetKind.Fancy)
    assert widget.summary() == "alpha:3"
    assert widget.bump() == 4
    assert widget.kind_name() == "fancy"
    easy_widget = sample.make_widget_easy("beta", value=2, kind=sample.WidgetKind_easy.Basic)
    assert easy_widget.summary() == "beta:2"
    assert isinstance(easy_widget, sample.Widget_easy)
    assert easy_widget.kind_name() == "basic"

    w2 = sample.Widget_easy("gamma", value=1, kind=sample.WidgetKind_easy.Fancy)
    assert w2.kind_name() == "fancy"


def test_exceptions() -> None:
    with pytest.raises(sample.SampleError):
        sample.raise_sample_error("boom")

    with pytest.raises(sample.WidgetError):
        sample.raise_widget_error("widget boom")

    assert issubclass(sample.WidgetError, sample.SampleError)
