#!/usr/bin/env python3
"""Create and push the next easybind release tag from the latest ``v*`` git tag.

Default: bump **patch** (0.2.3 → 0.2.4). Use ``--minor`` or ``--major`` to bump those
segments instead (resets lower segments to 0).

If there is no ``v*`` tag yet, starts from **v0.0.0** as the notional baseline, then bumps.

**GitHub vs PyPI:** pushing the tag triggers CI, but **PyPI upload can still fail**
(build error, twine, etc.). You may have a tag on GitHub **without** that version on PyPI
until you fix CI and re-run or tag again — the tag is not a guarantee of a good wheel.

Examples::

    ./scripts/release_tag.py --dry-run
    ./scripts/release_tag.py
    ./scripts/release_tag.py --minor
    ./scripts/release_tag.py --major --no-push

Requires a clean ``main`` sync (fetch/checkout/pull) unless ``--no-pull``.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

_REPO = Path(__file__).resolve().parents[1]
_SRC = _REPO / "src"
if str(_SRC) not in sys.path:
    sys.path.insert(0, str(_SRC))

from easybind.devtools.release_helpers import (  # noqa: E402
    ensure_clean_worktree,
    next_v_tag,
    tag_push_commands,
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

    if not (_REPO / ".git").exists() and not (_REPO / ".git").is_file():
        print(f"error: no git state at {_REPO}", file=sys.stderr)
        return 2

    latest, new_tag = next_v_tag(_REPO, level=ns.level)
    msg = f"easybind {new_tag.removeprefix('v')}"
    cmds = tag_push_commands(
        new_tag,
        remote=ns.remote,
        branch=ns.branch,
        no_pull=ns.no_pull,
        no_push=ns.no_push,
        tag_message=msg,
    )

    print(f"latest tag: {latest or '(none)'}")
    print(f"next tag:   {new_tag}  (bump {ns.level})")

    if ns.dry_run:
        print("--- dry-run; commands not executed ---")
        for c in cmds:
            print("+", " ".join(c))
        return 0

    ensure_clean_worktree(_REPO)
    for c in cmds:
        subprocess.run(c, cwd=_REPO, check=True)

    if ns.no_push:
        print(f"done: created {new_tag} locally (not pushed)")
    else:
        print(f"done: pushed {new_tag} (remote={ns.remote}, branch={ns.branch})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
