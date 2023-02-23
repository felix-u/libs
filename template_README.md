# projname

`projname` one-sentence summary.

![GIF/screenshot](./fix/this/path)

Licensed under [GPL-3.0](./LICENCE). To get `projname`,
[download the latest release](https://github.com/felix-u/projname/releases/)
or follow the [compilation instructions](#building).

*Disclaimer if needed: the syntax and features of `projname` are not yet stable, and are subject to change in upcoming
versions.*

### Summary

A paragraph or two explaining functionality.

### Examples

Example commands/output.

### Usage
```
projname

OPTIONS:
```

### Building

These dependencies are likely already present on any UNIX-like OS:

- `git`
- a C99 compiler, such as `gcc` or `clang`
- a `make` implementation, such as `gnumake`

Using `git`, clone the source code and navigate to the desired release, where `X.X` is the version number. Building
from the `master` branch is highly discouraged.
```sh
$ git clone link
$ git checkout vX.X
```

To compile an optimised binary at `./projname` relative to this repository's root directory, run:
```sh
$ make release
```

To compile this binary *and* copy it to `~/.local/bin`, run:
```sh
$ make install
```
