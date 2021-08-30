Basic library to reliably compute n-bit CRCs. The focus is on a readable, portable reference library, with reasonable performance as a secondary goal. 

**Status**
---
| Feature | Status | Notes |
| ------- | ------ | ----- |
| 1 < n < 8 | - [x] | Separate impl, not include-able yet |
| 9 < n < 16 | - [x] | Not include-able yet |
| 17 < n < 32 | - [x] | Not include-able yet |
| 33 < n < 64 | - [x] | Not include-able yet |
| Formally Verified | - [ ] | |
| Exhaustively Tested | - [ ] | |
| Production-Ready | - [ ] | |

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