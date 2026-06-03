#### 1.6.0
* Added scount CLI --show-all-files option to control file table display in output. [[Issue#59]](https://github.com/raven-computing/reckon/issues/59)
* Added scount CLI --show-files option to control file table display in output. [[Issue#59]](https://github.com/raven-computing/reckon/issues/59)
* Added support for C++ source format. [[Issue#4]](https://github.com/raven-computing/reckon/issues/4)
* Added support for LLC metric in R source files. [[Issue#44]](https://github.com/raven-computing/reckon/issues/44)
* Changed scount output behaviour to not show file table by default. [[Issue#59]](https://github.com/raven-computing/reckon/issues/59)
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.5.2...v1.6.0)

#### 1.5.2
* Added readStdinToCountStatistics() function.
* Improved scount input handling via standard input. Will now read input directly into memory instead of writing to a temporary file first.
* Removed createTempInputFileFromStdin() function.
* Removed removeTempInputFile() function.
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.5.1...v1.5.2)

#### 1.5.1
* Added fopenImpl() platform-specific function.
* Added initLogging() function.
* Fixed incorrectly drawn file table when file name contains multi-byte chars. [[Issue#55]](https://github.com/raven-computing/reckon/issues/55)
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.5.0...v1.5.1)

#### 1.5.0
* Added support for CMake format. [[Issue#49]](https://github.com/raven-computing/reckon/issues/49)
* Added support for HTML format. [[Issue#46]](https://github.com/raven-computing/reckon/issues/46)
* Added support for SQL format. [[Issue#45]](https://github.com/raven-computing/reckon/issues/45)
* Added support for YAML format. [[Issue#47]](https://github.com/raven-computing/reckon/issues/47)
* Added the rcnAllocCountStatistics() function. [[Issue#48]](https://github.com/raven-computing/reckon/issues/48)
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.4.0...v1.5.0)

#### 1.4.0
* Added support for CSS format. [[Issue#40]](https://github.com/raven-computing/reckon/issues/40)
* Added support for TypeScript source format. [[Issue#39]](https://github.com/raven-computing/reckon/issues/39)
* Added support in scount for reading source input via stdin. [[Issue#31]](https://github.com/raven-computing/reckon/issues/31)
* Added the --disable-LTO option to the build script, which interacts with the added RECKON_ENABLE_LTO CMake option to turn LTO off if requested. By default, LTO is now enabled for release build variants.
* Improved documentation.
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.3.1...v1.4.0)

#### 1.3.1
* Added support for handling .mjs and .cjs JavaScript file extensions.
* Fixed missing version tags in header documentation. 
* Improved accuracy of logical line count for C sources.
* Internal improvements.
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.3.0...v1.3.1)

#### 1.3.0
* Added support for JavaScript source format. [[Issue#3]](https://github.com/raven-computing/reckon/issues/3)
* Added the rcnCountLogicalLinesStrict() API function. [[Issue#38]](https://github.com/raven-computing/reckon/issues/38)
* Added the 'strict' flag to the RcnStatOptions struct. [[Issue#38]](https://github.com/raven-computing/reckon/issues/38)
* Added the '--strict' option to the scount CLI for strict syntax checking. [[Issue#38]](https://github.com/raven-computing/reckon/issues/38)
* Changed the behaviour of the rcnCountLogicalLines() API function to be lenient by default. [[Issue#38]](https://github.com/raven-computing/reckon/issues/38)
* Improved shown warnings for individual source files that produce errors when processed. [[Issue#25]](https://github.com/raven-computing/reckon/issues/25)
* Refactored some minor things internally.
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.2.0...v1.3.0)

#### 1.2.0
* Added support for Bash shell source format. [[Issue#2]](https://github.com/raven-computing/reckon/issues/2)
* Added support for R format. [[Issue#5]](https://github.com/raven-computing/reckon/issues/5)
* Added the '--lines' CLI option to scount to only show line-related metrics. [[Issue#23]](https://github.com/raven-computing/reckon/issues/23)
* Changed the file list limit to allow the processing of larger source trees. [[Issue#34]](https://github.com/raven-computing/reckon/issues/34)
* Changed LARGE_RESULT_THRESHOLD in scount print functionality to a 32 row threshold.
* Fixed typos in the documentation.
* Improved internal static assertions.
* Refactored printResultSingle() and printResultsMultiple() functions in scount.
* Removed explicit enumerator numeric values from RcnTextFormat.
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.1.0...v1.2.0)

#### 1.1.0
* Added support for Python format. [[Issue#1]](https://github.com/raven-computing/reckon/issues/1)
* Added support for JSON format. [[Issue#6]](https://github.com/raven-computing/reckon/issues/6)
* Added support for XML format. [[Issue#8]](https://github.com/raven-computing/reckon/issues/8)
* Added support for Markdown format. [[Issue#7]](https://github.com/raven-computing/reckon/issues/7)
* Added the --stop-on-error option to the scount CLI. [[Issue#11]](https://github.com/raven-computing/reckon/issues/11)
* Added the bool 'hasLogicalLines' field to the RcnCountResultGroup type.
* Changed the displayed text for LLC counts when the metric is not applicable from '0' (zero) to 'n/a'. [[Issue#17]](https://github.com/raven-computing/reckon/issues/17)
* Fixed minor typos in documentation.
* Improved error messages in scount to give more details about the specific file. [[Issue#22]](https://github.com/raven-computing/reckon/issues/22)
* Improved the appearance of the individual file table for large results. [[Issue#29]](https://github.com/raven-computing/reckon/issues/29)
* Improved API documentation of rcnMarkLogicalLinesInFile() function.
* Improved to project control code.
* Refactored logic for printing LLC count into separate function.
* See [full changelog](https://github.com/raven-computing/reckon/compare/v1.0.0...v1.1.0)

#### 1.0.0
* Open source release
