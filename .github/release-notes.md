#### Release notes:

* We added support for Python, Markdown, XML and JSON formats.
* Introduced the `--stop-on-error` option in the scount CLI to abort an operation as soon as an error is encountered.
* When the LLC metric is not applicable for a specific format, the result count in the LLC column will now be displayed as 'n/a' instead of the number zero.
* Added the boolean [hasLogicalLines](https://github.com/raven-computing/reckon/blob/ccfc74dd651b426648935426259cc56ee3b673b8/src/lib/include/reckon/reckon.h#L365-L376) field to the `RcnCountResultGroup` type in libreckon.
* Improved error messages in scount.
* Improved the appearance of the individual file table for large results.

See [Changelog](https://github.com/raven-computing/reckon/blob/v1.1.0/CHANGELOG.md).
