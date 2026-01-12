# API Contracts: Developer Package Creator

## Script Interface Contract

### create-dev-kit.sh Command Line Interface

```bash
# Usage
./scripts/create-dev-kit.sh [OPTIONS]

# Options
--help, -h              Show help message and exit
--version, -v           Show version information
--force, -f             Force rebuild of all artifacts
--output, -o FILE       Specify output package name (default: robot-dev-kit.tar.gz)
--verbose, -V            Enable verbose output
--no-build              Skip build check, fail if artifacts missing

# Return Codes
0   Success
1   Usage error
2   Build system error  
3   File system error
4   Validation error
5   Permission error
```

## Build System Interface Contract

### CMake Build Detection

```bash
# Function Signature
check_build_status() -> {status_code, status_message}

# Status Codes
0   Build up to date
1   Rebuild needed
2   Build in progress
3   Build system error

# Status Messages
"BUILD_UP_TO_DATE: All artifacts exist and are current"
"REBUILD_NEEDED: Build directory missing"
"REBUILD_NEEDED: Missing artifact {filename}"
"REBUILD_NEEDED: {filename} is out of date"
"BUILD_IN_PROGRESS: Another build is running"
"BUILD_ERROR: CMake configuration failed"
```

### Build Execution

```bash
# Function Signature
build_artifacts() -> {success, error_message}

# Success: boolean true if all artifacts built successfully
# Error Message: string with failure details if success is false
```

## Package Creation Interface Contract

### File Collection

```bash
# Function Signature
collect_package_contents() -> {file_list, total_size}

# File List: array of {source_path, target_path, type} objects
# Type enum: binary, source, header, documentation, template
# Total Size: number of bytes before compression
```

### Archive Creation

```bash
# Function Signature
create_package_archive(package_name, file_list) -> {success, package_size, error_message}

# Package Name: string - output filename
# Success: boolean true if archive created successfully
# Package Size: number of bytes in final archive
# Error Message: string with failure details if success is false
```

## Validation Interface Contract

### Package Validation

```bash
# Function Signature
validate_package(package_path) -> {valid, errors, warnings}

# Valid: boolean true if package passes all validation checks
# Errors: array of validation error messages
# Warnings: array of validation warning messages
```

### Required Content Validation

```bash
# Required Files Structure
REQUIRED_BINARIES=(
    "bin/robot-arena"
    "lib/librunner.so"
    "lib/libscanner.so"
)

REQUIRED_SOURCES=(
    "robots/Runner/Runner.cpp"
    "robots/Runner/Runner.h"
    "robots/Scanner/Scanner.cpp"
    "robots/Scanner/Scanner.h"
    "robots/Runner/CMakeLists.txt"
    "robots/Scanner/CMakeLists.txt"
)

REQUIRED_HEADERS=(
    "Robot.h"
)

REQUIRED_DOCUMENTATION=(
    "README.md"
    "MyRobot.cpp"
)
```

## Configuration Contract

### Script Configuration

```bash
# Default Configuration (can be overridden by environment variables)
PACKAGE_NAME="${PACKAGE_NAME:-robot-dev-kit.tar.gz}"
BUILD_DIR="${BUILD_DIR:-build}"
OUTPUT_DIR="${OUTPUT_DIR:-.}"
VERBOSE="${VERBOSE:-false}"
FORCE_REBUILD="${FORCE_REBUILD:-false}"
MAX_PACKAGE_SIZE="${MAX_PACKAGE_SIZE:-52428800}"  # 50MB
MAX_BUILD_TIME="${MAX_BUILD_TIME:-300}"         # 5 minutes
```

### Build Artifact Locations

```bash
# Source Paths (relative to repo root)
BUILD_BINARY_PATHS=(
    "build/robot-arena"
    "build/Robots/Runner/librunner.so"
    "build/Robots/Scanner/libscanner.so"
)

# Target Paths (within package)
PACKAGE_BINARY_PATHS=(
    "bin/robot-arena"
    "lib/librunner.so"
    "lib/libscanner.so"
)
```

## Error Handling Contract

### Error Categories

```bash
# Build Errors
ERROR_BUILD_MISSING=21
ERROR_BUILD_FAILED=22
ERROR_BUILD_TIMEOUT=23

# File System Errors
ERROR_FILE_NOT_FOUND=31
ERROR_PERMISSION_DENIED=32
ERROR_DISK_SPACE=33
ERROR_PACKAGE_CREATION=34

# Validation Errors
ERROR_VALIDATION_SIZE=41
ERROR_VALIDATION_CONTENT=42
ERROR_VALIDATION_INTEGRITY=43

# System Errors
ERROR_UNSUPPORTED_PLATFORM=51
ERROR_MISSING_DEPENDENCIES=52
```

### Error Message Format

```bash
# Standard Error Format
ERROR_[CATEGORY]_[TYPE]: [Human readable description]
Context: [Additional context information]
Suggestion: [How to resolve the error]

# Example
ERROR_BUILD_FAILED: CMake compilation failed for librunner.so
Context: Building in /tmp/workspace/build/Robots/Runner
Suggestion: Check compiler output above and fix compilation errors
```

## Logging Contract

### Log Levels

```bash
# Log Level Hierarchy (higher = more verbose)
LOG_LEVEL_ERROR=0
LOG_LEVEL_WARN=1
LOG_LEVEL_INFO=2
LOG_LEVEL_DEBUG=3

# Log Format
[YYYY-MM-DD HH:MM:SS] [LEVEL] MESSAGE
```

### Log Messages

```bash
# Key Log Events
log_info "Starting developer package creation"
log_debug "Checking build status..."
log_info "Build up to date, skipping compilation"
log_warn "Found outdated artifact: ${artifact}, rebuilding..."
log_error "Failed to create package: ${error_message}"
log_info "Package created successfully: ${package_path} (${size} bytes)"
```

## Testing Interface Contract

### Test Scenarios

```bash
# Function Signature for Testing
test_package_extraction(package_path, test_dir) -> {passed, failures}

# Test Cases
1. Extract package to temporary directory
2. Verify all expected files exist
3. Build Runner robot from extracted sources
4. Build Scanner robot from extracted sources
5. Build MyRobot template from extracted sources
6. Execute robot-arena with sample robots
7. Verify relative paths work from any directory
```

### Test Validation

```bash
# Success Criteria
- All files extract correctly
- All example robots compile without errors
- robot-arena executes successfully with sample robots
- Package can be extracted and used from any location
- Total build time for examples < 2 minutes
```

## Maintenance Contract

### Version Management

```bash
# Version Information
SCRIPT_VERSION="1.0.0"
COMPATIBILITY_VERSION="1.0.x"
SUPPORTED_PLATFORMS=("linux-gnu")

# Version Display Format
create-dev-kit.sh version ${SCRIPT_VERSION}
Compatible with RobotGame ${COMPATIBILITY_VERSION}
Supported platforms: ${SUPPORTED_PLATFORMS[*]}
```