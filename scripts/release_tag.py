#!/usr/bin/env python3
"""Create and push the next easybind release tag from the latest ``v*`` git tag.

Default: bump **patch** (0.2.3 → 0.2.4). Use ``--minor`` or ``--major`` to bump those
segments instead (resets lower segments to 0).

If there is no ``v*`` tag yet, starts from **v0.0.0** as the notional baseline, then bumps.

Examples::

    ./scripts/release_tag.py --dry-run
    ./scripts/release_tag.py
    ./scripts/release_tag.py --minor
    ./scripts/release_tag.py --major --no-push

Requires a clean ``main`` sync (fetch/checkout/pull) unless ``--no-pull``.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

_REPO = Path(__file__).resolve().parents[1]
_TAG_RE = re.compile(r"^v(\d+)\.(\d+)\.(\d+)$")


def _run(
    cmd: list[str],
    *,
    cwd: Path,
    check: bool = True,
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        cmd,
        cwd=cwd,
        check=check,
        text=True,
        capture_output=True,
    )


def _latest_version_tag(repo: Path) -> str | None:
    """Newest ``vMAJOR.MINOR.PATCH`` tag by version tuple (no reliance on git sort)."""
    p = _run(["git", "tag", "-l", "v*"], cwd=repo, check=False)
    if p.returncode != 0:
        return None
    tags: list[str] = []
    for line in p.stdout.splitlines():
        line = line.strip()
        if line and _TAG_RE.match(line):
            tags.append(line)
    if not tags:
        return None
    return max(tags, key=lambda t: _parse_semver(t))


def _parse_semver(tag: str) -> tuple[int, int, int]:
    m = _TAG_RE.fullmatch(tag.strip())
    if not m:
        raise ValueError(f"not a vMAJOR.MINOR.PATCH tag: {tag!r}")
    return int(m.group(1)), int(m.group(2)), int(m.group(3))


def _bump(
    major: int,
    minor: int,
    patch: int,
    *,
    level: str,
) -> tuple[int, int, int]:
    if level == "major":
        return major + 1, 0, 0
    if level == "minor":
        return major, minor + 1, 0
    return major, minor, patch + 1


def _format_tag(t: tuple[int, int, int]) -> str:
    return f"v{t[0]}.{t[1]}.{t[2]}"


def _ensure_clean(repo: Path) -> None:
    p = _run(["git", "status", "--porcelain"], cwd=repo, check=True)
    if p.stdout.strip():
        raise SystemExit(
            "working tree is not clean; commit or stash before tagging.\n"
            + p.stdout
        )


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    g = ap.add_mutually_exclusive_group()
    g.add_argument(
        "--patch",
        action="store_const",
        dest="level",
        const="patch",
        help="bump patch (default)",
    )
    g.add_argument(
        "--minor",
        action="store_const",
        dest="level",
        const="minor",
        help="bump minor, reset patch to 0",
    )
    g.add_argument(
        "--major",
        action="store_const",
        dest="level",
        const="major",
        help="bump major, reset minor and patch to 0",
    )
    ap.set_defaults(level="patch")
    ap.add_argument(
        "--dry-run",
        action="store_true",
        help="print the next tag and commands only; no git writes",
    )
    ap.add_argument(
        "--no-pull",
        action="store_true",
        help="do not fetch/checkout main/pull (still creates tag unless --dry-run)",
    )
    ap.add_argument(
        "--no-push",
        action="store_true",
        help="create the annotated tag locally but do not push main or the tag",
    )
    ap.add_argument(
        "--remote",
        default="origin",
        help="git remote for fetch/push (default: origin)",
    )
    ap.add_argument(
        "--branch",
        default="main",
        help="branch to update before tagging (default: main)",
    )
    ns = ap.parse_args()

    repo = _REPO
    if not (repo / ".git").exists() and not (repo / ".git").is_file():
        print(f"error: no git state at {repo}", file=sys.stderr)
        return 2

    latest = _latest_version_tag(repo)
    if latest is None:
        ma, mi, pa = 0, 0, 0
    else:
        ma, mi, pa = _parse_semver(latest)

    new_ver = _bump(ma, mi, pa, level=ns.level)
    new_tag = _format_tag(new_ver)

    print(f"latest tag: {latest or '(none)'}")
    print(f"next tag:   {new_tag}  (bump {ns.level})")

    cmds: list[list[str]] = []
    if not ns.no_pull:
        cmds.extend(
            [
                ["git", "fetch", ns.remote],
                ["git", "checkout", ns.branch],
                ["git", "pull", ns.remote, ns.branch],
            ]
        )
    cmds.append(["git", "tag", "-a", new_tag, "-m", f"easybind {new_tag.removeprefix('v')}"])
    if not ns.no_push:
        cmds.append(["git", "push", ns.remote, ns.branch])
        cmds.append(["git", "push", ns.remote, new_tag])

    if ns.dry_run:
        print("--- dry-run; commands not executed ---")
        for c in cmds:
            print("+", " ".join(c))
        return 0

    _ensure_clean(repo)

    for c in cmds:
        subprocess.run(c, cwd=repo, check=True)

    if ns.no_push:
        print(f"done: created {new_tag} locally (not pushed)")
    else:
        print(f"done: pushed {new_tag} (remote={ns.remote}, branch={ns.branch})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
