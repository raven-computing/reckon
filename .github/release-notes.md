#### Release notes:

* We added support for JavaScript source files.
* The behaviour of the [rcnCountLogicalLines()](https://github.com/raven-computing/reckon/blob/v1.3.0/src/lib/include/reckon/reckon.h#L898-L901) function was changed to be lenient with regards to partial syntax errors in source code when counting logical lines. Previously, an error for the file as a whole was reported for any syntax error. With the new behaviour, LLC reports are more accurate because sometimes the used source code parsers indicate unwarranted syntax errors due to technical limitations. Smaller errors are now tolerated and do not distort the overall count. The old behaviour can still be accessed via the newly introduced [rcnCountLogicalLinesStrict()](https://github.com/raven-computing/reckon/blob/v1.3.0/src/lib/include/reckon/reckon.h#L917-L920) function or by using the [strict](https://github.com/raven-computing/reckon/blob/v1.3.0/src/lib/include/reckon/reckon.h#L824-L834) field of the RcnStatOptions type.
* The above described behaviour is also reflected by `scount` and you can use the `--strict` CLI option to access the old behaviour.
* `scount` will now emit warnings in certain cases for individual files that could not be fully processed.

See [Changelog](https://github.com/raven-computing/reckon/blob/v1.3.0/CHANGELOG.md).
