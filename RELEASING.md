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

## CI upload

Pushing a tag matching `v*` triggers `.github/workflows/publish.yml`, which builds and uploads to PyPI.

Create a **repository secret** `PYPI_API_TOKEN` with a PyPI API token scoped to this project (or switch the workflow to [trusted publishing](https://docs.pypi.org/trusted-publishers/)).

## Platforms

The default `python -m build` on Linux produces a **single** `linux_x86_64` wheel (and an sdist). To ship **macOS / Windows / manylinux** variants, add **cibuildwheel** (or per-OS CI jobs) and upload all produced wheels in one release.

## Submodule / monorepo note

If this repo is a **git submodule** inside another project, **tags for versioning must exist on this repository’s own remote**, not only on the parent repo. Tag and release from the `easybind` repo (or push tags there after merging).
