#### Release notes:

* When scanning directories under a given path, subdirectories that are known to contain build-related files are now ignored so any build artifacts do not affect the line count metrics.
* All files with the hidden-attribute set on Windows are now ignored when scanning directories under a given path, similarly to how dotfiles are handled.

See [Changelog](https://github.com/raven-computing/reckon/blob/v1.6.1/CHANGELOG.md).
