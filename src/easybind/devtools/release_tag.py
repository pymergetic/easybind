"""Next semver ``v*`` tag and ``git fetch`` / ``tag`` / ``push`` (easybind repository)."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

from easybind.devtools.release_helpers import (
    ensure_clean_worktree,
    next_v_tag,
    tag_push_commands,
)


def main(argv: list[str] | None = None, *, repo: Path | None = None) -> int:
    """Entry point for ``easybind-release-tag``.

    *repo* defaults to the current working directory (run from the repository root).
    """
    ap = argparse.ArgumentParser(
        description=(
            "Create and push the next easybind release tag from the latest ``v*`` git tag.\n\n"
            "Default: bump **patch**. Use ``--minor`` / ``--major`` for other segments.\n"
            "If there is no ``v*`` tag yet, starts from **v0.0.0**, then bumps.\n\n"
            "**GitHub vs PyPI:** the tag appears immediately; PyPI upload can still fail."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--patch", action="store_const", dest="level", const="patch", help="bump patch (default)")
    g.add_argument("--minor", action="store_const", dest="level", const="minor", help="bump minor, reset patch")
    g.add_argument("--major", action="store_const", dest="level", const="major", help="bump major")
    ap.set_defaults(level="patch")
    ap.add_argument("--dry-run", action="store_true", help="print commands only; no git writes")
    ap.add_argument("--no-pull", action="store_true", help="skip fetch/checkout/pull")
    ap.add_argument("--no-push", action="store_true", help="tag locally only; do not push")
    ap.add_argument("--remote", default="origin", help="git remote (default: origin)")
    ap.add_argument("--branch", default="main", help="branch to update (default: main)")
    ap.add_argument(
        "--repo",
        type=Path,
        default=None,
        help="git repository root (default: current working directory)",
    )
    ns = ap.parse_args(argv)

    root = repo if repo is not None else ns.repo
    if root is None:
        root = Path.cwd()
    root = root.resolve()
    if not (root / ".git").exists() and not (root / ".git").is_file():
        print(f"error: no git state at {root}", file=sys.stderr)
        return 2

    latest, new_tag = next_v_tag(root, level=ns.level)
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

    ensure_clean_worktree(root)
    for c in cmds:
        subprocess.run(c, cwd=root, check=True)

    if ns.no_push:
        print(f"done: created {new_tag} locally (not pushed)")
    else:
        print(f"done: pushed {new_tag} (remote={ns.remote}, branch={ns.branch})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
