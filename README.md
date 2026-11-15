The base layer assumes an MSVC toolchain (can be installed with something like [PortableBuildTools](https://github.com/Data-Oriented-House/PortableBuildTools)) and the following file structure:
```
project_root/
    src/
        main.c
        base/
    build.c
    build.bat
```

All code in this repository is distributed under the Unlicense, **except for the following software in the `base/third_party` directory:**
- Inter, under the SIL Open Font License. See `base/third_party/Inter`
