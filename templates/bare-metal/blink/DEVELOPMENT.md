# Development Workflow

## Branch Strategy

| Branch | Purpose |
|--------|---------|
| `master` | Stable, release-ready firmware. Every commit on master is a verified release. |
| `develop` | Active development branch. All feature branches merge here first. |
| `for/develop/<name>` | Feature/task branches. Created from `develop`, merged back into `develop`. |

## Workflow

```
master ─────●───────────────●──────── (releases)
            │               ↑
develop ────●───●───●───●───● ─────── (integration)
                │       ↑
for/develop/ ───●───●───● ─────────── (feature work)
```

1. Create a feature branch from `develop`:
   ```bash
   git checkout develop
   git checkout -b for/develop/add-uart-driver
   ```

2. Work on the feature, commit often:
   ```bash
   git add src/uart.cpp
   git commit -m "feat: add UART driver for USART2"
   ```

3. Merge back into `develop` when ready:
   ```bash
   git checkout develop
   git merge for/develop/add-uart-driver
   git branch -d for/develop/add-uart-driver
   ```

4. When `develop` is stable and tested, merge into `master`:
   ```bash
   git checkout master
   git merge develop
   git tag -a v1.0.0 -m "Release v1.0.0"
   ```

## Commit Convention

Use [Conventional Commits](https://www.conventionalcommits.org/):

| Prefix | Usage |
|--------|-------|
| `feat:` | New feature or functionality |
| `fix:` | Bug fix |
| `refactor:` | Code restructuring without behavior change |
| `docs:` | Documentation only |
| `test:` | Adding or updating tests |
| `ci:` | CI/CD changes |
| `chore:` | Build scripts, dependencies, tooling |

## Release Checklist

- [ ] All features merged into `develop`
- [ ] Firmware builds without warnings
- [ ] Tested on target hardware
- [ ] Binary size within flash limits
- [ ] `develop` merged into `master`
- [ ] Release tagged (`vX.Y.Z`)
