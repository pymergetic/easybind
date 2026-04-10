"""Plan the next ``vMAJOR.MINOR.PATCH`` tag and ``git fetch`` / ``tag`` / ``push`` commands."""

from __future__ import annotations

import re
import subprocess
from pathlib import Path

_TAG_RE = re.compile(r"^v(\d+)\.(\d+)\.(\d+)$")


def _run(cmd: list[str], *, cwd: Path, check: bool = True) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd, check=check, text=True, capture_output=True)


def latest_v_tag(repo: Path) -> str | None:
    """Newest ``v*`` semver tag by version tuple."""
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


def _bump(major: int, minor: int, patch: int, *, level: str) -> tuple[int, int, int]:
    if level == "major":
        return major + 1, 0, 0
    if level == "minor":
        return major, minor + 1, 0
    return major, minor, patch + 1


def _format_tag(t: tuple[int, int, int]) -> str:
    return f"v{t[0]}.{t[1]}.{t[2]}"


def next_v_tag(repo: Path, *, level: str) -> tuple[str | None, str]:
    """Return ``(latest_tag_or_none, new_tag)`` for the given bump *level*."""
    latest = latest_v_tag(repo)
    if latest is None:
        ma, mi, pa = 0, 0, 0
    else:
        ma, mi, pa = _parse_semver(latest)
    new_tag = _format_tag(_bump(ma, mi, pa, level=level))
    return latest, new_tag


def tag_push_commands(
    new_tag: str,
    *,
    remote: str,
    branch: str,
    no_pull: bool,
    no_push: bool,
    tag_message: str,
) -> list[list[str]]:
    """Commands: optional fetch/checkout/pull, annotated tag, optional push branch + tag."""
    cmds: list[list[str]] = []
    if not no_pull:
        cmds.extend(
            [
                ["git", "fetch", remote],
                ["git", "checkout", branch],
                ["git", "pull", remote, branch],
            ]
        )
    cmds.append(["git", "tag", "-a", new_tag, "-m", tag_message])
    if not no_push:
        cmds.append(["git", "push", remote, branch])
        cmds.append(["git", "push", remote, new_tag])
    return cmds


def ensure_clean_worktree(repo: Path) -> None:
    p = _run(["git", "status", "--porcelain"], cwd=repo, check=True)
    if p.stdout.strip():
        raise SystemExit(
            "working tree is not clean; commit or stash before tagging.\n" + p.stdout
        )
