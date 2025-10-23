# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

### Changed

### Deprecated

### Removed

### Fixed

### Security

## [2.2.1]

No changes in the code beside a little fix for the use of `delete` and the addition of a header file with semantic versioning macros.
It mostly focuses on the modernizations of the build system.

### Added

- [ci] added workflow with vcpkg on windows, linux, and macos
- [cmake] added presets and vcpkg toolchain support
- [cmake] proper export of targets so that the library can be consumed by other cmake projects
- [cmake] added more aggressive compiler flags for warnings
- [cmake] option to build the documentation with doxygen
- [doc] this changelog
- a version header with semantic versioning macros 

### Changed

- [cmake] modernized cmake files
- [cmake] set minimum required cmake version to 3.20
- [cmake] set c++ standard to 11

### Fixed

- properly use `delete[]` for arrays (compiler warning)

## [2.2.0] - 2017-06-05

- support for reading OpenCV calibration files (requires boost)
- runtime-configurable code
- non templated, reusable library
- faster compilation times
- interface for tracking multiple individual markers
- Hull based tracking for multi marker mode
- cleaned up and easier to use interface

## [2.1.1] - ?

- easier C++ based API
- support for 4096 binary based markers - no need to design new markers yourself
- supports RGB565, Gray images
- "Robust Planar Pose" algorithm that reduces jitter while tracking
- uses modern text-file based camera calibration
- Vignetting Compensation
- automatic Thresholding
- Hull based tracking in multi marker mode