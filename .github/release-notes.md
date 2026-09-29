#### Release notes:

* This release comes with a massive performance improvement due to the newly added parallelization capabilities. Files can now be processed in parallel by using multiple threads. `scount` does this by default when applicable and library users can control the behaviour with the useMultiThreading option.
* Python bytecode files are now completely ignored by ignoring '\_\_pycache\_\_' directories altogether.
* For library users, we have marked API functions in `libreckon` explicitly as MT-safe.
* We fixed a bug where count annotations for certain file extensions did not work when the input was passed in over standard-input with a specified file extension, e.g. `-.cpp`.
* Improvements of the UX.

See [Changelog](https://github.com/raven-computing/reckon/blob/v1.9.0/CHANGELOG.md).
