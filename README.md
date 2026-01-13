# Reckon: A Tool to Count Logical Lines of Code

Reckon is a tool for computing various size-based source code metrics, including counting logical lines of code (LLC) across source trees. The `scount` application is intended to be used on the command-line, whereas the `libreckon` library provides a C API to access the functionality programmatically. 

As opposed to the more traditional physical lines of code metric, logical lines of code focuses on how many *meaningful* lines exist. For example, a single statement would count as one logical line of code, regardless of how many physical lines the statemenet spans across. This makes the LLC metric more unaffected by arbitrary style-related decisions that can be different between software projects.

## Getting Started

To install the `scount` command-line application, download the corresponding package for your system from the [Release Page](https://github.com/raven-computing/reckon/releases) and install it.

To use `libreckon` in your own project, add the sources to your build as a dependency. If you use CMake, the easiest way to do that is by using the [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) functionality.

## Compatibility

Reckon supports the following platforms:
* Debian-based GNU/Linux distributions
* Windows

Compatibility of the CLI and API is managed according to semantic versioning. Please note that the library does not guarantee a stable ABI as it should only be used as a project-level dependency, not a system-level dependency.

## Documentation

Please refer to the [project documentation](https://docs.raven-computing.com/reckon/latest) page.

## Build

To build this project from source you require a compiler for **C17** or higher.  
We use **CMake** as our build system generator. This project requires **CMake 3.22** or higher.  
We support compiling with **GCC** and **Clang** on Debian-based GNU/Linux distributions and with **MSVC** and **GCC via MSYS2** on Windows.

Use the `build.sh` script to build all targets:
```
./build.sh
```
See `build.sh --help` for available options.

To build the documentation resources, minimum Doxygen v1.14 is required.

## Development

The project offers Visual Studio Code integration, but of course you're free to use whatever editor or IDE you're comfortable with. The easiest way to get started is to simply clone this source repository to your local workstation, open the project directory in VS Code as a workspace and then import the provided profile from `.vscode/Reckon.code-profile`.

When building or developing on Windows, you do need to have a Bash shell available, e.g. via a Git Bash or via MSYS2.

### Tests

Execute the entire test suite by running the `test.sh` script:
```
./test.sh
```
See `test.sh --help` for available options.

## License

This project is licensed under Apache License 2.0.
