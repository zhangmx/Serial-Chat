# CI/CD Setup Guide

This document explains how to configure GitHub Actions for building and releasing Serial Chat.

## Overview

The project uses GitHub Actions for:
- Building the application on Windows, macOS, and Linux
- Creating release artifacts (installers, AppImage, etc.)
- Publishing releases to GitHub Releases

## Workflow Status

**Current Status: DISABLED**

The workflow is configured for manual trigger only (`workflow_dispatch`). This means it won't run automatically on push or pull requests until you explicitly enable it.

## Setup Instructions

### Step 1: Repository Settings

1. Go to your GitHub repository
2. Navigate to **Settings** → **Actions** → **General**
3. Under "Workflow permissions", select:
   - **Read and write permissions**
   - Check "Allow GitHub Actions to create and approve pull requests"
4. Click **Save**

### Step 2: Enable Automatic Builds (Optional)

To enable automatic builds on tags and pull requests, edit `.github/workflows/build.yml`:

```yaml
on:
  workflow_dispatch:
    inputs:
      version:
        description: 'Version to release (e.g., 1.0.0)'
        required: false
        default: ''
  
  # Uncomment these lines to enable automatic builds:
  push:
    tags:
      - 'v*'
  
  pull_request:
    branches: [ main, develop ]
```

### Step 3: Manual Trigger

To manually trigger a build:

1. Go to **Actions** tab in your repository
2. Select "Build and Release" workflow
3. Click **Run workflow**
4. Optionally enter a version number
5. Click **Run workflow**

### Step 4: Creating a Release

#### Option A: Tag-based Release (Recommended)

```bash
# Create and push a tag
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

This will trigger the build workflow (if enabled) and create a draft release.

#### Option B: Manual Release

1. Go to **Actions** → **Build and Release**
2. Click **Run workflow**
3. Enter the version (e.g., `1.0.0`)
4. Wait for builds to complete
5. Go to **Releases** and publish the draft release

## Version Control Design

### Versioning Scheme

We use [Semantic Versioning](https://semver.org/):
- **MAJOR.MINOR.PATCH** (e.g., 1.0.0)
- MAJOR: Breaking changes
- MINOR: New features, backward compatible
- PATCH: Bug fixes, backward compatible

### Tag Protection (Recommended)

To prevent unauthorized version releases:

1. Go to **Settings** → **Tags**
2. Click **New rule**
3. Set tag pattern: `v*`
4. Under "Tag protection":
   - Restrict who can create matching tags
   - Select specific users/teams who can create releases
5. Click **Create**

### Branch Protection

Recommended branch protection for `main`:

1. Go to **Settings** → **Branches**
2. Click **Add rule**
3. Branch name pattern: `main`
4. Enable:
   - Require pull request reviews before merging
   - Require status checks to pass before merging
   - Require branches to be up to date before merging
5. Click **Create**

## Release Workflow

### Recommended Process

1. **Development** happens on feature branches
2. **Merge** to `develop` branch for integration testing
3. **Create release branch** from `develop`:
   ```bash
   git checkout -b release/1.0.0 develop
   ```
4. **Update version** in `CMakeLists.txt` and `MainWindow.h`:
   ```cmake
   project(SerialChat VERSION 1.0.0 LANGUAGES CXX)
   ```
   ```cpp
   #define APP_VERSION "1.0.0"
   ```
5. **Update CHANGELOG.md**
6. **Merge** release branch to `main`
7. **Tag** the release:
   ```bash
   git checkout main
   git merge release/1.0.0
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin main --tags
   ```
8. **Delete** release branch:
   ```bash
   git branch -d release/1.0.0
   ```
9. **Merge** `main` back to `develop`:
   ```bash
   git checkout develop
   git merge main
   git push origin develop
   ```

## Secrets Configuration

The workflow uses the default `GITHUB_TOKEN` which is automatically provided by GitHub Actions. No additional secrets are required for basic functionality.

### Optional: Code Signing

For Windows code signing, add these secrets:
- `WINDOWS_CERTIFICATE`: Base64-encoded .pfx certificate
- `WINDOWS_CERTIFICATE_PASSWORD`: Certificate password

For macOS code signing:
- `MACOS_CERTIFICATE`: Base64-encoded .p12 certificate
- `MACOS_CERTIFICATE_PASSWORD`: Certificate password
- `MACOS_NOTARIZATION_APPLE_ID`: Apple ID for notarization
- `MACOS_NOTARIZATION_PASSWORD`: App-specific password

## Troubleshooting

### Build Fails with Qt Not Found

Ensure the Qt version in the workflow matches what's available:
```yaml
env:
  QT_VERSION: '5.15.2'  # Check available versions
```

### Windows Build Fails

Check that MSVC is properly installed:
```yaml
arch: 'win64_msvc2019_64'  # or win64_msvc2017_64
```

### Linux AppImage Fails

Ensure all dependencies are installed:
```yaml
- name: Install dependencies
  run: |
    sudo apt-get update
    sudo apt-get install -y libgl1-mesa-dev ...
```

### Permission Denied

Check repository settings:
- Settings → Actions → General → Workflow permissions
- Ensure "Read and write permissions" is selected

## Artifacts

Successful builds produce these artifacts:
- `SerialChat-Windows-x64.zip` - Windows portable package
- `SerialChat-macOS.dmg` - macOS disk image
- `SerialChat-Linux-x86_64.AppImage` - Linux AppImage

## Related Files

- `.github/workflows/build.yml` - GitHub Actions workflow
- `CMakeLists.txt` - Build configuration
- `src/ui/MainWindow.h` - Version constants
- `CHANGELOG.md` - Version history (create if needed)
