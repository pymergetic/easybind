"""Bump ``{distribution}~=…`` compatible-release pins in a ``pyproject.toml`` string or file."""

from __future__ import annotations

import argparse
import json
import re
import sys
import time
from pathlib import Path
from urllib.error import HTTPError
from urllib.request import urlopen

# PEP 440 version suitable for ``~=`` RHS in typical pyproject pins (numeric release).
_VERSION_RE = re.compile(r"^[0-9]+(?:\.[0-9]+)*$")

# Reject pathological / mistaken distribution strings (full re.escape covers the rest).
_DIST_OK = re.compile(r"^[a-zA-Z0-9][a-zA-Z0-9._-]*$")


def _pin_pattern(distribution: str) -> re.Pattern[str]:
    if not _DIST_OK.match(distribution):
        raise ValueError(
            f"invalid distribution name for pin replacement: {distribution!r} "
            "(expected a PyPI-style name, e.g. easybind, cppdantic, scikit-build-core)"
        )
    return re.compile(rf"({re.escape(distribution)}~=)([0-9]+(?:\.[0-9]+)*)")


def fetch_pypi_version(package: str = "easybind", *, timeout_s: float = 30.0) -> str:
    """Return ``info.version`` from ``https://pypi.org/pypi/{package}/json``."""
    url = f"https://pypi.org/pypi/{package}/json"
    with urlopen(url, timeout=timeout_s) as r:
        data = json.load(r)
    return str(data["info"]["version"])


def pypi_release_exists(package: str, version: str, *, timeout_s: float = 15.0) -> bool:
    """Return True if ``https://pypi.org/pypi/{package}/{version}/json`` exists (release uploaded)."""
    url = f"https://pypi.org/pypi/{package}/{version}/json"
    try:
        with urlopen(url, timeout=timeout_s) as r:
            return getattr(r, "status", 200) == 200
    except HTTPError as e:
        if e.code == 404:
            return False
        raise


def compatible_pin_versions(pyproject_toml: str, distribution: str) -> list[str]:
    """Return every version string from ``{distribution}~=VERSION`` pins (order preserved)."""
    pat = _pin_pattern(distribution)
    return [m.group(2) for m in pat.finditer(pyproject_toml)]


def single_compatible_pin_version(
    pyproject_toml: str,
    distribution: str,
    *,
    empty_pins_suffix: str = "",
) -> str:
    """Return the version string if all ``{distribution}~=…`` pins agree; else raise ``ValueError``."""
    vers = compatible_pin_versions(pyproject_toml, distribution)
    if not vers:
        raise ValueError(
            f"no `{distribution}~=...` pins in pyproject.toml{empty_pins_suffix}"
        )
    uniq = set(vers)
    if len(uniq) != 1:
        raise ValueError(
            f"{distribution}~= pins disagree: {sorted(uniq)!r}; fix pyproject.toml first"
        )
    return vers[0]


def wait_pypi_for_compatible_pin(
    pyproject_toml: str,
    distribution: str,
    *,
    timeout_s: float = 1800.0,
    interval_s: float = 30.0,
    verbose: bool = False,
) -> tuple[str, int]:
    """Poll PyPI until ``pypi_release_exists(distribution, version)`` for the pin.

    Uses ``single_compatible_pin_version`` on *pyproject_toml*. Returns
    ``(resolved_version, attempt_count)``. Raises ``TimeoutError`` if the release
    never appears within *timeout_s*. With *verbose*, log progress to stdout.
    """
    version = single_compatible_pin_version(pyproject_toml, distribution)
    if verbose:
        print(
            f"waiting for {distribution}=={version} on PyPI "
            f"(timeout {timeout_s:.0f}s, interval {interval_s:.0f}s)...",
            flush=True,
        )
    deadline = time.monotonic() + timeout_s
    attempt = 0
    while True:
        attempt += 1
        if pypi_release_exists(distribution, version):
            return version, attempt
        if time.monotonic() >= deadline:
            raise TimeoutError(
                f"timed out waiting for {distribution}=={version} on PyPI "
                f"after {timeout_s}s ({attempt} attempts)"
            )
        if verbose:
            remaining = int(max(0, deadline - time.monotonic()))
            print(
                f"attempt {attempt}: not yet (retry in {interval_s:.0f}s, ~{remaining}s left)...",
                flush=True,
            )
        time.sleep(interval_s)


def installed_distribution_version(package: str = "easybind") -> str:
    """Return ``importlib.metadata.version(package)`` for the active environment."""
    from importlib.metadata import version

    return version(package)


def bump_compatible_pins(pyproject_toml: str, distribution: str, version: str) -> tuple[str, int]:
    """Replace each ``{distribution}~=X.Y.Z`` with ``{distribution}~={version}``.

    Returns ``(new_text, replacement_count)``.
    """
    if not _VERSION_RE.match(version):
        raise ValueError(f"invalid PEP 440 version for pin: {version!r}")

    pat = _pin_pattern(distribution)

    def repl(m: re.Match[str]) -> str:
        return f"{m.group(1)}{version}"

    new_text, n = pat.subn(repl, pyproject_toml)
    return new_text, n


def bump_compatible_pins_in_file(
    pyproject: Path | str,
    distribution: str,
    version: str,
    *,
    dry_run: bool = False,
) -> int:
    """Read ``pyproject.toml``, apply :func:`bump_compatible_pins`, write back unless ``dry_run``.

    Returns the number of replacements. Raises ``ValueError`` if no matching pin exists.
    """
    path = Path(pyproject)
    text = path.read_text(encoding="utf-8")
    new_text, n = bump_compatible_pins(text, distribution, version)
    if n == 0:
        raise ValueError(f"no `{distribution}~=...` pin found in {path}")
    if new_text == text:
        return n
    if not dry_run:
        path.write_text(new_text, encoding="utf-8")
    return n


def bump_easybind_compatible_pins(pyproject_toml: str, version: str) -> tuple[str, int]:
    """Same as :func:`bump_compatible_pins` with ``distribution=\"easybind\"``."""
    return bump_compatible_pins(pyproject_toml, "easybind", version)


def bump_easybind_compatible_pins_in_file(
    pyproject: Path | str,
    version: str,
    *,
    dry_run: bool = False,
) -> int:
    """Same as :func:`bump_compatible_pins_in_file` with ``distribution=\"easybind\"``."""
    return bump_compatible_pins_in_file(pyproject, "easybind", version, dry_run=dry_run)


def main(argv: list[str] | None = None) -> int:
    """CLI: ``easybind-pin-pyproject`` (see ``--help``)."""
    ap = argparse.ArgumentParser(
        description=(
            "Set every {distribution}~= pin in pyproject.toml "
            "(default distribution: easybind). "
            "Default version is latest on PyPI — that can lag a git tag or a wheel still uploading; "
            "use --version X.Y.Z when you already know the release, or --installed to match "
            "the package in the current environment (e.g. editable install from the release commit)."
        )
    )
    ap.add_argument(
        "--distribution",
        "-d",
        default="easybind",
        metavar="NAME",
        help="PyPI distribution name to match in pins (default: easybind), e.g. cppdantic, easybind",
    )
    ap.add_argument(
        "--pyproject",
        type=Path,
        default=None,
        help="path to pyproject.toml (default: ./pyproject.toml under cwd)",
    )
    ap.add_argument(
        "--version",
        metavar="X.Y.Z",
        default=None,
        help="pin to this exact release (use when PyPI does not list it yet, e.g. CI still publishing)",
    )
    ap.add_argument(
        "--installed",
        action="store_true",
        help="use importlib.metadata.version for --distribution in this env (e.g. after pip install -e ../easybind)",
    )
    ap.add_argument("--dry-run", action="store_true", help="do not write the file")
    ns = ap.parse_args(argv)

    if ns.version and ns.installed:
        print("error: use only one of --version or --installed", file=sys.stderr)
        return 2

    pyproject = ns.pyproject if ns.pyproject is not None else Path.cwd() / "pyproject.toml"
    if not pyproject.is_file():
        print(f"error: {pyproject} not found", file=sys.stderr)
        return 2

    dist = ns.distribution.strip()
    if not dist:
        print("error: empty --distribution", file=sys.stderr)
        return 2

    if ns.installed:
        ver = installed_distribution_version(dist)
    elif ns.version:
        ver = ns.version.strip()
    else:
        ver = fetch_pypi_version(dist)

    if not _VERSION_RE.match(ver):
        print(f"error: bad version string: {ver!r}", file=sys.stderr)
        return 2

    try:
        n = bump_compatible_pins_in_file(pyproject, dist, ver, dry_run=ns.dry_run)
    except ValueError as e:
        print(f"error: {e}", file=sys.stderr)
        return 1

    action = "would update" if ns.dry_run else "updated"
    print(f"{action} {n} {dist}~= pin(s) to ~={ver} in {pyproject}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
