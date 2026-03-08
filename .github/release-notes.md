#### Release notes:

* We added support for Bash shell code and R source files.
* With `scount` you can now use the `--lines` option to only compute and show line-related metrics.
* `scount` will now show more rows in the table listing for individual files.
* We have increased some internal file processing limits so that larger source trees are handled without interference. This is a safety mechanism to protect against symlink loops in the file system. Normally, you should not have to worry about hitting this limit.
* Other internal improvements.

See [Changelog](https://github.com/raven-computing/reckon/blob/v1.2.0/CHANGELOG.md).
