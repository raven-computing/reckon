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
