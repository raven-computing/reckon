# Reckon

This is the official documentation for the Reckon project and the `libreckon` API reference.

## Introduction

Reckon is a tool to count things in source files. Predominantly, it is used to count the number of logical lines of code, but it also provides other general metrics. The purpose is to give an insight on the size of a source file or an entire source tree of a software project. Logical lines of code are a superior metric compared to the traditional number of physical lines of code. While the measurement results of latter heavily depends on the specific formatting and coding style used in source files, counting the number of logical lines is unaffected from subjective style-related aspects and physical line breaks. For differerent source trees written in the same programming language, and to some extend even different programming languages, a comparison becomes more viable.

Reckon provides:

- **`scount`**: A command-line tool to compute source code size metrics for a single file or an entire directory tree.
- **`libreckon`**: A C library that exposes an API for the same counting functionality.

## Installation

Precompiled binaries are being provided for all supported platforms. Please check the [Release Page](https://github.com/raven-computing/reckon/releases) to download the package of your choice.

Please note that we only provide builds for the `scount` command-line application. If you want to use `libreckon` in your project, you must use the sources directly or create your own builds.

If you want to build and install from source, please see the source repository [Readme section](https://github.com/raven-computing/reckon?tab=readme-ov-file#build).

## Metrics

Reckon computes the following metrics:

- **LLC - Logical Lines of Code**:  
Language-aware count of top-level statement/declaration units.
- **PHL - Physical Lines**:  
Hard physical lines, including blank lines and comments.
- **WRD - Words**:  
Non-empty sequences of printable characters delimited by whitespace.
- **CHR - Characters**:  
Unicode code points, including control characters such as newlines.
- **SZE - Source Size**:  
File size in bytes.

For the full technical definition of LLC, see the `libreckon` documentation in the `reckon.h` header.

The definition of any metric is strictly speaking not part of the CLI and API contract. Both the exact definitions as well as the library implementation may evolve in future releases, such that different versions of both the scount command-line application as well as the Reckon library may compute slightly different results for a particular metric and input combination. In particular, the LLC metric may slightly change in future releases to adjust what is counted as a logical line of code. These adjustments are not considered breaking changes.

## Compatibility

Reckon uses semantic versioning. This applies to both the CLI as well as the C API.  

Unless constrained by specific command-line options (we are working on those), the output of `scount` is not part of the CLI contract and therefore might change between versions. The output is intended to be consumed by human users.

Since Reckon must be classified as a project-level dependency, as opposed to a system-level dependency, the ABI is not guaranteed to be stable and may break between versions. As a result, any project that adds `libreckon` as a dependency must rebuild after every update.

### Supported Formats

At the time of writing, the library supports the processing of the following file formats:

- C
- Java
- Plain text (`.txt`-style unformatted text)

Support for more formats is under way.

### Supported Encodings

Reckon supports processing text encoded as:

- UTF-8
- UTF-16

For UTF-16, a BOM must be present to indicate endianness. For UTF-8, a BOM may or may not be present, although, UTF-8 BOMs are generally discouraged. If a given text does not exhibit any BOM, it is asasumed to be encoded in UTF-8.  
If an input has encoding errors, operations complete gracefully but the computed results are undefined and may therefore not be accurate.

## Library Usage

The high-level workflow is:

1. Create a `RcnCountStatistics` struct for a file or directory path with `rcnCreateCountStatistics()`.
2. Configure options with `RcnStatOptions`.
3. Call `rcnCount()` to perform the operation.
4. Read results from `RcnCountStatistics` (totals, per-format arrays, and per-file result sets).
5. Free resources with `rcnFreeCountStatistics()`.
