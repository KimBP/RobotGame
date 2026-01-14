#!/bin/bash

# Developer Package Creator Script
# Creates distributable package with everything needed for robot development

set -euo pipefail

# Script Configuration
SCRIPT_VERSION="1.0.0"
COMPATIBILITY_VERSION="1.0.x"
SUPPORTED_PLATFORMS=("linux-gnu")
PACKAGE_NAME="${PACKAGE_NAME:-robot-dev-kit.tar.gz}"
BUILD_DIR="${BUILD_DIR:-build}"
OUTPUT_DIR="${OUTPUT_DIR:-$(pwd)}"
VERBOSE="${VERBOSE:-false}"
FORCE_REBUILD="${FORCE_REBUILD:-false}"
MAX_PACKAGE_SIZE="${MAX_PACKAGE_SIZE:-52428800}"  # 50MB
MAX_BUILD_TIME="${MAX_BUILD_TIME:-300}"         # 5 minutes

# Determine script and repository directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Error Codes
ERROR_BUILD_MISSING=21
ERROR_BUILD_FAILED=22
ERROR_BUILD_TIMEOUT=23
ERROR_FILE_NOT_FOUND=31
ERROR_PERMISSION_DENIED=32
ERROR_DISK_SPACE=33
ERROR_PACKAGE_CREATION=34
ERROR_VALIDATION_SIZE=41
ERROR_VALIDATION_CONTENT=42
ERROR_VALIDATION_INTEGRITY=43
ERROR_UNSUPPORTED_PLATFORM=51
ERROR_MISSING_DEPENDENCIES=52

# Log Levels
LOG_LEVEL_ERROR=0
LOG_LEVEL_WARN=1
LOG_LEVEL_INFO=2
LOG_LEVEL_DEBUG=3

# Logging Functions
log_error() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] [ERROR] $1" >&2
}

log_warn() {
    if [[ "${VERBOSE}" == "true" ]] || [[ "${LOG_LEVEL_WARN}" -ge "${LOG_LEVEL_INFO}" ]]; then
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] [WARN] $1" >&2
    fi
}

log_info() {
    if [[ "${VERBOSE}" == "true" ]] || [[ "${LOG_LEVEL_INFO}" -ge "${LOG_LEVEL_INFO}" ]]; then
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] [INFO] $1"
    fi
}

log_debug() {
    if [[ "${VERBOSE}" == "true" ]] || [[ "${LOG_LEVEL_DEBUG}" -ge "${LOG_LEVEL_INFO}" ]]; then
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] [DEBUG] $1"
    fi
}

# Display help information
show_help() {
    cat << 'EOF'
Developer Package Creator v1.0.0
Creates distributable package with everything needed for robot development

USAGE:
    ./scripts/create-dev-kit.sh [OPTIONS]

OPTIONS:
    -h, --help              Show this help message and exit
    -v, --version           Show version information and exit
    -f, --force             Force rebuild of all artifacts
    -o, --output FILE       Specify output package name (default: robot-dev-kit.tar.gz)
    -V, --verbose            Enable verbose output
    --no-build              Skip build check, fail if artifacts missing

EXAMPLES:
    ./scripts/create-dev-kit.sh
    ./scripts/create-dev-kit.sh --output my-robot-kit.tar.gz
    ./scripts/create-dev-kit.sh --force --verbose

EXIT CODES:
    0   Success
    1   Usage error
    2   Build system error  
    3   File system error
    4   Validation error
    5   Permission error
EOF
}

# Display version information
show_version() {
    echo "create-dev-kit.sh version ${SCRIPT_VERSION}"
    echo "Compatible with RobotGame ${COMPATIBILITY_VERSION}"
    echo "Supported platforms: ${SUPPORTED_PLATFORMS[*]}"
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -v|--version)
                show_version
                exit 0
                ;;
            -f|--force)
                FORCE_REBUILD=true
                shift
                ;;
            -o|--output)
                if [[ -z "${2:-}" ]]; then
                    log_error "Option $1 requires an argument"
                    exit 1
                fi
                PACKAGE_NAME="$2"
                shift 2
                ;;
            -V|--verbose)
                VERBOSE=true
                shift
                ;;
            --no-build)
                NO_BUILD=true
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Check if build artifacts exist and are up to date
check_build_status() {
    # Change to repository root for file checks
    local original_dir=$(pwd)
    cd "$REPO_ROOT"
    
    local artifacts=("$BUILD_DIR/robot-arena" "$BUILD_DIR/Robots/Runner/librunner.so" "$BUILD_DIR/Robots/Scanner/libscanner.so")
    
    # Check if build directory exists
    if [[ ! -d "$BUILD_DIR" ]]; then
        cd "$original_dir"
        echo "1"  # Rebuild needed
        return
    fi
    
    # Check if all artifacts exist
    for artifact in "${artifacts[@]}"; do
        if [[ ! -f "$artifact" ]]; then
            cd "$original_dir"
            echo "1"  # Rebuild needed
            return
        fi
    done
    
    # Check if sources are newer than artifacts
    local cmake_files=("CMakeLists.txt" "Robots/Runner/CMakeLists.txt" "Robots/Scanner/CMakeLists.txt")
    local source_files=("Robot.h" "Robot.cpp" "Robots/Runner/Runner.cpp" "Robots/Runner/Runner.h" "Robots/Scanner/Scanner.cpp" "Robots/Scanner/Scanner.h")
    
    for artifact in "${artifacts[@]}"; do
        for source_file in "${cmake_files[@]}" "${source_files[@]}"; do
            if [[ -f "$source_file" ]] && [[ "$source_file" -nt "$artifact" ]]; then
                cd "$original_dir"
                echo "1"  # Rebuild needed
                return
            fi
        done
    done
    
    cd "$original_dir"
    echo "0"  # Build up to date
}

# Build artifacts using CMake
build_artifacts() {
    log_info "Building missing or outdated artifacts..."
    
    # Change to repository root
    local original_dir=$(pwd)
    cd "$REPO_ROOT"
    
    # Create build directory if it doesn't exist
    if [[ ! -d "$BUILD_DIR" ]]; then
        mkdir -p "$BUILD_DIR"
    fi
    
    # Configure with CMake
    cd "$BUILD_DIR"
    if ! cmake ..; then
        log_error "CMake configuration failed"
        cd "$original_dir"
        echo "false"
        return
    fi
    
    # Build the project
    if ! cmake --build . --parallel $(nproc 2>/dev/null || echo "4"); then
        log_error "CMake build failed"
        cd "$original_dir"
        echo "false"
        return
    fi
    
    cd "$original_dir"
    log_info "Build completed successfully"
    echo "true"
}

# Collect files for package
collect_package_contents() {
    local temp_dir="$1"
    local file_list=()
    
    # Change to repository root for file operations
    local original_dir=$(pwd)
    cd "$REPO_ROOT"
    
    # Create package structure
    mkdir -p "$temp_dir/bin"
    mkdir -p "$temp_dir/lib"
    mkdir -p "$temp_dir/include"
    mkdir -p "$temp_dir/robots/Runner"
    mkdir -p "$temp_dir/robots/Scanner"
    
    # Copy binary artifacts with validation
    log_info "Collecting binary artifacts..."
    local binary_artifacts=("$BUILD_DIR/robot-arena" "$BUILD_DIR/Robots/Runner/librunner.so" "$BUILD_DIR/Robots/Scanner/libscanner.so")
    local target_binaries=("bin/robot-arena" "lib/librunner.so" "lib/libscanner.so")
    
    for i in "${!binary_artifacts[@]}"; do
        local src="${binary_artifacts[$i]}"
        local dst="$temp_dir/${target_binaries[$i]}"
        
        if [[ ! -f "$src" ]]; then
            log_error "Binary artifact not found: $src"
            cd "$original_dir"
            return 1
        fi
        
        cp "$src" "$dst"
        log_debug "Copied $src to $dst"
    done
    
    # Copy source files
    log_info "Collecting robot source code..."
    local source_files=(
        "Robots/Runner/Runner.cpp"
        "Robots/Runner/Runner.h" 
        "Robots/Scanner/Scanner.cpp"
        "Robots/Scanner/Scanner.h"
    )
    
    for src_file in "${source_files[@]}"; do
        if [[ ! -f "$src_file" ]]; then
            log_error "Source file not found: $src_file"
            cd "$original_dir"
            return 1
        fi
        cp "$src_file" "$temp_dir/robots/${src_file#Robots/}"
        log_debug "Copied $src_file"
    done
    
    # Copy and fix CMakeLists.txt files
    log_info "Collecting and fixing CMakeLists.txt files..."
    local cmake_files=(
        "Robots/Runner/CMakeLists.txt"
        "Robots/Scanner/CMakeLists.txt"
    )
    
    for cmake_file in "${cmake_files[@]}"; do
        if [[ ! -f "$cmake_file" ]]; then
            log_error "CMakeLists.txt not found: $cmake_file"
            cd "$original_dir"
            return 1
        fi
        
        # Copy CMakeLists.txt and fix include paths
        local target_file="$temp_dir/robots/${cmake_file#Robots/}"
        cp "$cmake_file" "$target_file"
        
        # Replace CMAKE_SOURCE_DIR with appropriate include path
        sed -i 's|${CMAKE_SOURCE_DIR}|${CMAKE_SOURCE_DIR}/../../include|g' "$target_file"
        log_debug "Copied and fixed $cmake_file"
    done
    
    # Copy include headers
    log_info "Collecting include headers..."
    local include_files=("Logger.h" "RobotTypes.h" "Trigonometry.h" "safequeue.h")
    
    for include_file in "${include_files[@]}"; do
        if [[ ! -f "$include_file" ]]; then
            log_error "Include header not found: $include_file"
            cd "$original_dir"
            return 1
        fi
        cp "$include_file" "$temp_dir/include/"
        log_debug "Copied $include_file to include/"
    done
    
    # Copy main headers and documentation
    log_info "Collecting headers and documentation..."
    local doc_files=("docs/dev-kit/README.md" "docs/dev-kit/MyRobot.cpp" "docs/dev-kit/ROBOT_RULES.md")
    
    # Copy Robot.h to include directory
    if [[ ! -f "Robot.h" ]]; then
        log_error "Robot.h not found"
        cd "$original_dir"
        return 1
    fi
    cp "Robot.h" "$temp_dir/include/"
    log_debug "Copied Robot.h to include/"
    
    for doc_file in "${doc_files[@]}"; do
        if [[ ! -f "$doc_file" ]]; then
            log_error "Documentation file not found: $doc_file"
            cd "$original_dir"
            return 1
        fi
        cp "$doc_file" "$temp_dir/"
        log_debug "Copied $doc_file"
    done
    
    # Calculate total size and validate
    local total_size=$(du -sb "$temp_dir" | cut -f1)
    log_debug "Total package size: $total_size bytes"
    
    # Return to original directory
    cd "$original_dir"
    
    # Check size limit during collection
    if [[ $total_size -gt $MAX_PACKAGE_SIZE ]]; then
        log_error "Package size ($total_size bytes) exceeds maximum ($MAX_PACKAGE_SIZE bytes)"
        return 1
    fi
    
    echo "$total_size"
}

# Create tar.gz archive
create_package_archive() {
    local temp_dir="$1"
    local output_file="$2"
    
    log_info "Creating package archive: $output_file"
    
    # Get the current working directory (where script was called from)
    local original_dir=$(pwd)
    
    # Determine output directory and filename
    local output_dir
    local output_basename
    
    if [[ "$output_file" == /* ]]; then
        # Absolute path - use as is
        output_dir=$(dirname "$output_file")
        output_basename=$(basename "$output_file")
    else
        # Relative path - use original directory
        output_dir="$original_dir"
        output_basename="$output_file"
    fi
    
    # Change to temp directory for relative paths
    cd "$temp_dir"
    
    # Create tar.gz archive in the specified directory
    if ! tar -czf "$output_dir/$output_basename" --exclude="..* --exclude=./..*" *; then
        log_error "Failed to create tar.gz archive"
        echo "false"
        cd "$original_dir"
        return
    fi
    
    cd "$original_dir"
    
    # Get package size - handle both absolute and relative paths
    local package_size
    if [[ "$output_file" == /* ]]; then
        package_size=$(stat -c%s "$output_file" 2>/dev/null || echo "0")
    else
        package_size=$(stat -c%s "$output_file" 2>/dev/null || echo "0")
    fi
    
    # Validate package size
    if [[ $package_size -gt $MAX_PACKAGE_SIZE ]]; then
        log_error "Package size ($package_size bytes) exceeds maximum ($MAX_PACKAGE_SIZE bytes)"
        echo "false"
        return
    fi
    
    log_info "Package created successfully: $output_file ($package_size bytes)"
    echo "$package_size"
}

    # Validate package contents
    validate_package() {
        local package_file="$1"
        local temp_dir="$2"
        local errors=()
        
        log_info "Validating package contents..."
        
        # Extract package to temp directory
        if ! tar -xzf "$package_file" -C "$temp_dir"; then
            errors+=("Failed to extract package")
            printf '%s\n' "${errors[@]}"
            return 1
        fi
        
        # Check required binaries
        local required_bins=("bin/robot-arena" "lib/librunner.so" "lib/libscanner.so")
        for bin in "${required_bins[@]}"; do
            if [[ ! -f "$temp_dir/$bin" ]]; then
                errors+=("Missing binary: $bin")
            fi
        done
        
        # Check required sources
        local required_sources=("robots/Runner/Runner.cpp" "robots/Runner/Runner.h" "robots/Scanner/Scanner.cpp" "robots/Scanner/Scanner.h" "Robot.h")
        for source in "${required_sources[@]}"; do
            if [[ ! -f "$temp_dir/$source" ]]; then
                errors+=("Missing source: $source")
            fi
        done
        
        # Check required documentation
        local required_docs=("README.md" "MyRobot.cpp" "ROBOT_RULES.md")
        for doc in "${required_docs[@]}"; do
            if [[ ! -f "$temp_dir/$doc" ]]; then
                errors+=("Missing documentation: $doc")
            fi
        done
        
        # Validate binary executability
        if [[ ! -x "$temp_dir/bin/robot-arena" ]]; then
            errors+=("robot-arena is not executable")
        fi
        
        printf '%s\n' "${errors[@]}"
        return 0
    }

# Main script entry point
main() {
    log_info "Starting developer package creation"
    log_debug "Script version: ${SCRIPT_VERSION}"
    
    parse_arguments "$@"
    
    # Check build status
    local build_status=$(check_build_status)
    
    if [[ "${FORCE_REBUILD}" == "true" ]] || [[ "${build_status}" == "1" ]]; then
        if [[ "${NO_BUILD:-}" == "true" ]]; then
            log_error "Build required but --no-build specified"
            exit 2
        fi
        
        local build_result=$(build_artifacts)
        if [[ "${build_result}" != "true" ]]; then
            log_error "Build failed, aborting package creation"
            exit 2
        fi
    else
        log_info "Build artifacts are up to date"
    fi
    
    # Create temporary directory for package
    local temp_dir=$(mktemp -d)
    trap "rm -rf $temp_dir" EXIT
    
    # Collect package contents
    local total_size=$(collect_package_contents "$temp_dir")
    log_info "Collected $total_size bytes for packaging"
    
    # Create package archive
    local package_result=$(create_package_archive "$temp_dir" "$PACKAGE_NAME")
    if [[ "${package_result}" == "false" ]]; then
        log_error "Package creation failed"
        exit 3
    fi
    
    log_info "Developer package created successfully: $PACKAGE_NAME"
    log_info "Package is ready for distribution"
    
    # Validate package
    validate_package() {
        local package_file="$1"
        local temp_dir="$2"
        local errors=()
        
        log_info "Validating package contents..."
        
        # Extract package to temp directory
        if ! tar -xzf "$package_file" -C "$temp_dir"; then
            errors+=("Failed to extract package")
            printf '%s\n' "${errors[@]}"
            return 1
        fi
        
        # Check required binaries
        local required_bins=("bin/robot-arena" "lib/librunner.so" "lib/libscanner.so")
        for bin in "${required_bins[@]}"; do
            if [[ ! -f "$temp_dir/$bin" ]]; then
                errors+=("Missing binary: $bin")
            fi
        done
        
        # Check required sources
        local required_sources=("robots/Runner/Runner.cpp" "robots/Runner/Runner.h" "robots/Scanner/Scanner.cpp" "robots/Scanner/Scanner.h" "Robot.h")
        for source in "${required_sources[@]}"; do
            if [[ ! -f "$temp_dir/$source" ]]; then
                errors+=("Missing source: $source")
            fi
        done
        
        # Check required include headers
        local required_includes=("include/Logger.h" "include/RobotTypes.h" "include/Trigonometry.h" "include/Robot.h" "include/safequeue.h")
        for include in "${required_includes[@]}"; do
            if [[ ! -f "$temp_dir/$include" ]]; then
                errors+=("Missing include header: $include")
            fi
        done
        
        # Check required documentation
        local required_docs=("README.md" "MyRobot.cpp" "ROBOT_RULES.md")
        for doc in "${required_docs[@]}"; do
            if [[ ! -f "$temp_dir/$doc" ]]; then
                errors+=("Missing documentation: $doc")
            fi
        done
        
        # Validate binary executability
        if [[ ! -x "$temp_dir/bin/robot-arena" ]]; then
            errors+=("robot-arena is not executable")
        fi
        
        printf '%s\n' "${errors[@]}"
        return 0
    }
}

# Test function for manual validation
test_package_extraction() {
    local package_file="$1"
    local test_dir="$2"
    
    log_info "Testing package extraction..."
    
    # Extract package
    if ! tar -xzf "$package_file" -C "$test_dir"; then
        log_error "Package extraction failed"
        return 1
    fi
    
    # Check required files
    local required_files=("bin/robot-arena" "lib/librunner.so" "lib/libscanner.so" "README.md" "MyRobot.cpp" "ROBOT_RULES.md" "include/Logger.h" "include/RobotTypes.h" "include/Trigonometry.h" "include/Robot.h" "include/safequeue.h")
    for file in "${required_files[@]}"; do
        if [[ ! -f "$test_dir/$file" ]]; then
            log_error "Missing file in package: $file"
            return 1
        fi
    done
    
    log_info "Package extraction test passed"
    return 0
}

main "$@"
