# Version Control and Release Design

This document describes the versioning strategy and release process for Serial Chat.

## Version Numbering

Serial Chat uses [Semantic Versioning 2.0.0](https://semver.org/):

```
MAJOR.MINOR.PATCH
```

- **MAJOR**: Incompatible API changes, major feature overhauls
- **MINOR**: New features in a backward-compatible manner
- **PATCH**: Backward-compatible bug fixes

### Examples
- `1.0.0` → `1.0.1`: Bug fix
- `1.0.1` → `1.1.0`: New feature (e.g., console dock)
- `1.1.0` → `2.0.0`: Major UI overhaul or breaking changes

## Version Location

Version is defined in these locations (must be kept in sync):

1. **CMakeLists.txt** (line 2):
   ```cmake
   project(SerialChat VERSION 1.0.0 LANGUAGES CXX)
   ```

2. **src/ui/MainWindow.h** (lines 22-25):
   ```cpp
   #define APP_VERSION "1.0.0"
   #define APP_VERSION_MAJOR 1
   #define APP_VERSION_MINOR 0
   #define APP_VERSION_PATCH 0
   ```

3. **src/main.cpp** (line 11):
   ```cpp
   app.setApplicationVersion("1.0.0");
   ```

### Version Sync Script

Consider creating a script to update all version locations:

```bash
#!/bin/bash
# update-version.sh
VERSION=$1

# Update CMakeLists.txt
sed -i "s/project(SerialChat VERSION .* LANGUAGES/project(SerialChat VERSION $VERSION LANGUAGES/" CMakeLists.txt

# Update MainWindow.h
MAJOR=$(echo $VERSION | cut -d. -f1)
MINOR=$(echo $VERSION | cut -d. -f2)
PATCH=$(echo $VERSION | cut -d. -f3)

sed -i "s/#define APP_VERSION \".*\"/#define APP_VERSION \"$VERSION\"/" src/ui/MainWindow.h
sed -i "s/#define APP_VERSION_MAJOR .*/#define APP_VERSION_MAJOR $MAJOR/" src/ui/MainWindow.h
sed -i "s/#define APP_VERSION_MINOR .*/#define APP_VERSION_MINOR $MINOR/" src/ui/MainWindow.h
sed -i "s/#define APP_VERSION_PATCH .*/#define APP_VERSION_PATCH $PATCH/" src/ui/MainWindow.h

# Update main.cpp
sed -i "s/setApplicationVersion(\".*\")/setApplicationVersion(\"$VERSION\")/" src/main.cpp
```

## Git Tag Strategy

### Tag Format
- Release tags: `v1.0.0`, `v1.1.0`, `v2.0.0`
- Pre-release tags: `v1.0.0-alpha.1`, `v1.0.0-beta.2`, `v1.0.0-rc.1`

### Creating Tags

```bash
# Create annotated tag (recommended)
git tag -a v1.0.0 -m "Release version 1.0.0"

# Push tag
git push origin v1.0.0

# Push all tags
git push origin --tags
```

### Deleting Tags (if needed)

```bash
# Delete local tag
git tag -d v1.0.0

# Delete remote tag
git push origin :refs/tags/v1.0.0
```

## Tag Protection

### GitHub Tag Protection

To prevent unauthorized releases:

1. Go to repository **Settings** → **Tags**
2. Click **New rule**
3. Tag name pattern: `v*`
4. Restrict who can create matching tags:
   - Select maintainers only
   - Or specific users/teams

### Who Can Create Release Tags?

By default, anyone with push access can create tags. With tag protection:
- Only designated maintainers can create `v*` tags
- This prevents accidental or unauthorized releases

## Branch Strategy

```
main ─────●─────●─────●─────●─────●───→ (stable releases only)
          │     ↑     │     ↑     │
          │     │     │     │     │
develop ──●──●──●──●──●──●──●──●──●───→ (development integration)
          │  │  ↑  │  │  ↑  │  │  │
          │  │  │  │  │  │  │  │  │
feature ──●──●──┘  │  │  │  │  │  │
                   │  │  │  │  │  │
feature ───────────●──●──┘  │  │  │
                            │  │  │
release/1.1.0 ──────────────●──┘  │
                                  │
hotfix/1.0.1 ─────────────────────●
```

### Branch Types

| Branch | Purpose | Base | Merge To |
|--------|---------|------|----------|
| `main` | Stable releases | - | - |
| `develop` | Integration | `main` | `main` |
| `feature/*` | New features | `develop` | `develop` |
| `release/*` | Release prep | `develop` | `main`, `develop` |
| `hotfix/*` | Emergency fixes | `main` | `main`, `develop` |

## Release Process

### 1. Prepare Release

```bash
# Start from develop
git checkout develop
git pull origin develop

# Create release branch
git checkout -b release/1.1.0

# Update version numbers
./update-version.sh 1.1.0

# Update CHANGELOG.md
# Add release notes

# Commit version bump
git add .
git commit -m "Bump version to 1.1.0"
```

### 2. Test Release

```bash
# Build and test
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build -C Release
```

### 3. Finalize Release

```bash
# Merge to main
git checkout main
git pull origin main
git merge --no-ff release/1.1.0 -m "Merge release 1.1.0"

# Create tag
git tag -a v1.1.0 -m "Release version 1.1.0"

# Push
git push origin main
git push origin v1.1.0

# Merge back to develop
git checkout develop
git merge --no-ff main -m "Merge main after release 1.1.0"
git push origin develop

# Delete release branch
git branch -d release/1.1.0
```

### 4. GitHub Release

1. The CI/CD workflow creates a draft release
2. Go to **Releases** in GitHub
3. Edit the draft release
4. Add detailed release notes
5. Click **Publish release**

## Hotfix Process

For urgent fixes on released versions:

```bash
# Create hotfix from main
git checkout main
git checkout -b hotfix/1.0.1

# Fix the issue
# Update version to 1.0.1
./update-version.sh 1.0.1

git add .
git commit -m "Fix critical bug XYZ"
git commit -m "Bump version to 1.0.1"

# Merge to main
git checkout main
git merge --no-ff hotfix/1.0.1
git tag -a v1.0.1 -m "Hotfix 1.0.1"
git push origin main v1.0.1

# Merge to develop
git checkout develop
git merge --no-ff main
git push origin develop

# Delete hotfix branch
git branch -d hotfix/1.0.1
```

## CHANGELOG.md Format

```markdown
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added
- Feature in development

## [1.1.0] - 2026-01-15

### Added
- Console dock widget for debug output
- Connect All menu option
- Group message target selection
- Application icon

### Changed
- Title and status labels converted to interactive buttons
- Improved search with case-insensitive matching

### Fixed
- Group selection now properly updates chat widget

## [1.0.0] - 2026-01-01

### Added
- Initial release
- Serial port communication with chat interface
- Group chat with message forwarding
- Message history persistence
```

## Automated Version Checking

Add to CI/CD to verify version consistency:

```yaml
- name: Check version consistency
  run: |
    CMAKE_VERSION=$(grep "project(SerialChat VERSION" CMakeLists.txt | sed 's/.*VERSION \([0-9.]*\).*/\1/')
    HEADER_VERSION=$(grep "#define APP_VERSION " src/ui/MainWindow.h | sed 's/.*"\(.*\)".*/\1/')
    
    if [ "$CMAKE_VERSION" != "$HEADER_VERSION" ]; then
      echo "Version mismatch: CMakeLists.txt=$CMAKE_VERSION, MainWindow.h=$HEADER_VERSION"
      exit 1
    fi
```
