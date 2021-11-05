Basic library to reliably compute n-bit CRCs. The focus is on a readable, portable reference library, with reasonable performance as a secondary goal. 

**Status**
---
| Feature             | Status             | Notes                   |
|---------------------|--------------------|-------------------------|
| 1 < n < 8           | :x:                | Not Currently Supported |
| 9 < n < 16          | :heavy_check_mark: | Functional              |
| 17 < n < 32         | :heavy_check_mark: | Functional              |
| 33 < n < 64         | :heavy_check_mark: | Functional              |
| Formally Verified   | :x:                |                         |
| Exhaustively Tested | :x:                |                         |
| Production-Ready    | :heavy_check_mark: |                         |

**Build Dependencies**
---
* CMake >= 3.0.0
* C99 compliant compiler

**Testing Dependencies**
---
* CMake >= 3.0.0
* [Verified Software Toolchain](https://vst.cs.princeton.edu/)

**Build Instructions**
---

1. Create an output folder for an out-of-source build

2. `cd` to the output folder and invoke cmake with appropriate options.
    - Set `-DADDRESS_SANITIZER=1` or `-DUNDEFINED_SANITIZER=1` to build with
    sanitizers

3. `make` (or other build system invocation)

**Useful Rules**

1. `make format`

2. `make tidy`