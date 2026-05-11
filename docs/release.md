# Release process

## Versioning policy

Releases use SemVer (`vMAJOR.MINOR.PATCH`). The project is currently pre-1.0:
patch and minor releases may both introduce small source-side changes. Where
exactly the line falls between "patch" and "minor" is at the maintainer's
discretion until the API stabilises (target: v1.0).

Practical advice for downstream projects:

- Pin a specific tag in `stmproject.toml` (`[sdk] version = "0.1.2"`) instead of
  `develop`.
- Before bumping, read the [upgrade notes](migration.md) for source-side changes.
- Use `stmtool sdk update --version <tag>` to refresh the cache in lockstep.

## Tooling

The version number is derived from git tags via `setuptools-scm`. There is no
hand-edited version constant anywhere — tagging the repository is what creates
a release for `stmtool` consumers.

`tools/stmtool/pyproject.toml` configures:

```toml
[tool.setuptools_scm]
root = "../.."
version_file = "stmtool/_version.py"
```

## Release procedure

1. Make sure `develop` is green on CI (build matrix on F407VG/F401CE/F411CE).
2. Locally preview docs with `mkdocs serve`.
3. Tag the merge commit on `develop`:
   ```bash
   git tag -a v0.1.3 -m "Release v0.1.3" <commit>
   git push origin v0.1.3
   ```
4. Create the GitHub Release using notes from this page's "Release history".
5. The `docs.yml` workflow rebuilds the site automatically; verify
   <https://khosta77.github.io/stm32-sdk/> picks up the new content.

## Release history

### v0.1.3 (preparing)

Highlights:

- New documentation site (MkDocs Material), English and Russian.
- New `stmtool sdk update`, `sdk list-versions`, `sdk path` subcommands.
- Optional `CLAUDE.md` generation per-template via `--with-claude`.
- All new sensors (MPU6050, SSD1306, W25Q32) and DMA stream wrapper documented.
- I2C multi-byte read fix per RM0090 §27.3.3, valid at 400 kHz.
- `GpioConfig` rewritten as an aggregate with `consteval gpio({...})` validator.

See [upgrade notes](migration.md) for source-side changes required.

### v0.1.2

- `stmtool` install hardening (`install.sh` clears cache on reinstall).

### v0.1.1

- FreeRTOS templates (`freertos/blink`, `freertos/mpu6050-uart`) registered for
  `stmtool` discovery.

### v0.1.0

- Initial release. `setuptools-scm` versioning for `stmtool` and the SDK as a
  whole. CI moved from `main` to `develop`.

## GitHub Pages

The documentation site is published from the `gh-pages` branch (created by the
`docs.yml` workflow). The site is available at
<https://khosta77.github.io/stm32-sdk/>. After the first deployment, enable
Pages in repository Settings → Pages → Source = `gh-pages` / `(root)`.
