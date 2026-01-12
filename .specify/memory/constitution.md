<!--
Sync Impact Report:
Version change: 0.0.0 → 1.0.0 (initial adoption)
List of modified principles: All 5 principles newly defined for RobotGame project
Added sections: Technology Stack, Build & Deployment
Removed sections: None
Templates requiring updates: ✅ plan-template.md, ✅ spec-template.md, ✅ tasks-template.md
Follow-up TODOs: None - all placeholders filled with project-specific values
-->

# RobotGame Constitution

## Core Principles

### I. Plugin Architecture
Robot behaviors MUST be implemented as shared library plugins with standard interface; Each plugin contains exactly one Robot class implementing pure virtual methods; Plugins are dynamically loaded at runtime via getRobot() factory function; Robot plugins MUST be self-contained with no external dependencies beyond core framework

### II. Real-time Event System
All game components communicate via event-driven scheduling; Robot actions queue through Scheduler for deterministic tick-based execution; Events must be thread-safe using SafeQueue for cross-thread communication; No direct component-to-component calls - all communication mediated through Scheduler

### III. Hardware Abstraction
Robot interactions MUST go through RobCtrl interface abstraction; Sensor data (scan, position, status) accessed via controlled getter methods; Actuator commands (drive, cannon, energy management) rate-limited through interface; Direct hardware or system calls prohibited in robot implementations

### IV. Simulation Integrity
Game physics MUST be deterministic and reproducible; All timing derived from single tick counter; Random elements eliminated or seeded for reproducibility; State changes only occur through validated Scheduler events

### V. Visual Observability
Game state MUST be observable via SDL2-based Viewer; All robot positions, cannon shots, and energy levels displayed in real-time; Logging through thread-safe Logger with configurable debug output; Visual and textual logging synchronized for debugging

## Technology Stack

C++17 standard required for all core components; CMake 3.28+ for build system with compile_commands.json generation; SDL2 ecosystem (SDL2, SDL2_image, SDL2_ttf, SDL2_gfx) for visualization; POSIX threads for concurrent Logger and Viewer; Dynamic loading (dlopen/dlsym) for robot plugin system

## Build & Deployment

Modular CMake structure with subdirectories for robot plugins; Hidden symbol visibility for plugins (CMAKE_CXX_VISIBILITY_PRESET hidden); Static analysis through clangd using compile_commands.json; Cross-platform Linux compatibility required; Simple make-based build from CMake output

## Governance

Constitution supersedes all coding guidelines and architectural decisions; Amendments require unanimous approval from core maintainers; All pull requests must validate compliance via automated constitution checks; Complexity increases must be justified with performance or maintainability benefits; Technical debt must be documented and scheduled for resolution

Scripts must not use 'heredocs'

Files must not be duplicated in the repo.

**Version**: 1.0.0 | **Ratified**: 2025-01-09 | **Last Amended**: 2025-01-09
