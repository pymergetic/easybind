#!/usr/bin/env python3
"""Poll PyPI until a release matches the compatible-release pins in ``pyproject.toml``.

Reads ``{distribution}~=X.Y.Z`` pins (see ``easybind.devtools``), requires one
consistent version, then polls until ``https://pypi.org/pypi/{distribution}/{version}/json``
exists or a timeout. Use in downstream CI when you tag immediately after releasing
the dependency and PyPI lags behind git.

Example (cppdantic repo, submodule at ``external/easybind``)::

    PYTHONPATH=external/easybind/src python external/easybind/scripts/wait_pypi_release.py

Defaults: ``distribution=easybind``, 30 min timeout, 30 s interval.
"""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

# Local tree: ``easybind`` package lives under ``../src`` (no pip install required).
_ROOT = Path(__file__).resolve().parents[1]
_SRC = _ROOT / "src"
if str(_SRC) not in sys.path:
    sys.path.insert(0, str(_SRC))

from easybind.devtools.pin_pyproject import (  # noqa: E402
    pypi_release_exists,
    single_compatible_pin_version,
)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--pyproject",
        type=Path,
        default=Path("pyproject.toml"),
        help="path to pyproject.toml (default: ./pyproject.toml)",
    )
    ap.add_argument(
        "--distribution",
        default="easybind",
        help="PyPI distribution name to match in ``{name}~=…`` pins (default: easybind)",
    )
    ap.add_argument(
        "--timeout",
        type=int,
        default=1800,
        metavar="SEC",
        help="give up after this many seconds (default: 1800)",
    )
    ap.add_argument(
        "--interval",
        type=int,
        default=30,
        metavar="SEC",
        help="seconds between checks (default: 30)",
    )
    ns = ap.parse_args()

    pp = ns.pyproject.resolve()
    if not pp.is_file():
        print(f"error: not a file: {pp}", file=sys.stderr)
        return 2

    text = pp.read_text(encoding="utf-8")
    try:
        version = single_compatible_pin_version(text, ns.distribution)
    except ValueError as e:
        raise SystemExit(str(e)) from None

    dist = ns.distribution
    print(
        f"waiting for {dist}=={version} on PyPI "
        f"(timeout {ns.timeout}s, interval {ns.interval}s)...",
        flush=True,
    )

    deadline = time.monotonic() + ns.timeout
    attempt = 0
    while True:
        attempt += 1
        if pypi_release_exists(dist, version):
            print(f"ok: {dist} {version} is on PyPI (attempt {attempt})", flush=True)
            return 0
        if time.monotonic() >= deadline:
            print(
                f"error: timed out waiting for {dist}=={version} on PyPI "
                f"after {ns.timeout}s ({attempt} attempts). "
                "Confirm the dependency’s release workflow finished and PyPI has the artifacts.",
                file=sys.stderr,
            )
            return 1
        remaining = int(max(0, deadline - time.monotonic()))
        print(
            f"attempt {attempt}: not yet (retry in {ns.interval}s, ~{remaining}s left)...",
            flush=True,
        )
        time.sleep(ns.interval)


if __name__ == "__main__":
    raise SystemExit(main())
