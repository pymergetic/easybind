# Releasing to PyPI

## How the version is chosen

- The **PyPI distribution** and **`pip install`** name are **`easybind`**, matching **`import easybind`**.
- The **version string** is computed automatically by **setuptools-scm** from **Git**:
  - Tag the release commit with **`vMAJOR.MINOR.PATCH`** (leading `v` is supported), e.g. `v0.1.0`.
  - On **exactly** that tagged commit, the wheel/sdist version is **`0.1.0`** (with `no-guess-dev` + `no-local-version`, suitable for PyPI).
  - Untagged / between tags you get a **development** version like `0.0.1.post1.devN` (fallback + distance). Fine for test uploads to TestPyPI; for production PyPI, **always build from a clean tree on a release tag**.

## One-off upload (manual)

```bash
python -m pip install build twine
git fetch --tags
python -m build        # produces dist/*.whl and dist/*.tar.gz
twine check dist/*
twine upload dist/*    # or TestPyPI: twine upload --repository testpypi dist/*
```

Configure credentials with **API token** (`~/.pypirc` or environment variables) or use **trusted publishing** (OIDC) from GitHub Actions.

## Helper: next semver tag + push

`scripts/release_tag.py` picks the latest **`vMAJOR.MINOR.PATCH`** tag, bumps **patch** by default (or **`--minor`** / **`--major`**), then **`fetch` / `checkout` / `pull`** (unless **`--no-pull`**), creates an **annotated** tag, and **`push`**es branch + tag (unless **`--no-push`** or **`--dry-run`**).

```bash
./scripts/release_tag.py --dry-run
./scripts/release_tag.py
./scripts/release_tag.py --minor
./scripts/release_tag.py --major
```

Run from the **easybind** repo root. Requires a **clean** working tree.

## CI upload

Pushing a tag matching `v*` triggers `.github/workflows/publish.yml`, which builds and uploads to PyPI.

Create a **repository secret** `PYPI_API_TOKEN` with a PyPI API token scoped to this project (or switch the workflow to [trusted publishing](https://docs.pypi.org/trusted-publishers/)).

## Platforms

**CI** (`.github/workflows/publish.yml`) builds an **sdist** plus **manylinux** wheels via **cibuildwheel**, using the **`manylinux_2_28`** image (`manylinux-x86_64-image` in `pyproject.toml`). PyPI accepts those wheels (e.g. `manylinux_2_28_x86_64`).

There is **no** official PyPA Docker image for **`manylinux_2_27`**; the closest policy with maintained images is **`manylinux_2_28`** (glibc 2.28+). For broader Linux compatibility (older glibc), switch the cibuildwheel image to **`manylinux2014`** in `[tool.cibuildwheel]`.

Locally, a plain `python -m build` on Linux may still emit a bare `linux_x86_64` wheel; do not upload that to PyPI. **macOS / Windows** wheels are not built in CI yet; add matrix jobs or **cibuildwheel** targets if needed.

## Submodule / monorepo note

If this repo is a **git submodule** inside another project, **tags for versioning must exist on this repository’s own remote**, not only on the parent repo. Tag and release from the `easybind` repo (or push tags there after merging).

## Downstream: cppdantic

The **cppdantic** demo package depends on **easybind** with a compatible-release pin (`easybind~=A.B.C` in its `pyproject.toml`). **Release easybind to PyPI first**, then bump those pins in the cppdantic repo to the new **`A.B.C`** before tagging cppdantic. See **cppdantic**’s **`RELEASING.md`** for the full order.
