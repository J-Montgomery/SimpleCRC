[![License](https://img.shields.io/github/license/J-Montgomery/SimpleCRC)](https://github.com/J-Montgomery/SimpleCRC/blob/master/LICENSE)
![Latest Release](https://img.shields.io/github/v/release/J-Montgomery/SimpleCRC)

# Introduction

SimpleCRC is a basic library to reliably compute n-bit CRCs. The focus is on
having a readable, general, and portable reference library with reasonable
performance as a secondary goal. This library is distinguished from similar
projects like [crcany](https://github.com/madler/crcany) because the focus is on
providing a universal library that can be configured at runtime, rather than
generating specialized code for each individual CRC or providing a command line
application to do so.

- [Introduction](#introduction)
  - [Feature Status](#feature-status)
  - [Feature Roadmap](#feature-roadmap)
  - [Resolved](#resolved)
- [Dependencies](#dependencies)
  - [Build Dependencies](#build-dependencies)
  - [Dev Dependencies](#dev-dependencies)
- [Build Instructions](#build-instructions)
- [API Reference](#api-reference)
    - [Macros](#macros)
    - [Structs](#structs)
    - [Functions](#functions)
    - [Example Usages](#example-usages)
    - [Minimizing Runtime Footprint](#minimizing-runtime-footprint)
- [Useful Development Rules](#useful-development-rules)
  - [Fuzzer Testing](#fuzzer-testing)
- [Implementation Details](#implementation-details)


## Feature Status
| Feature             | Status             | Notes                   |
|---------------------|--------------------|-------------------------|
| 1 < n < 8           | :x:                | Not Currently Supported |
| 9 < n < 16          | :heavy_check_mark: | Functional              |
| 17 < n < 32         | :heavy_check_mark: | Functional              |
| 33 < n < 64         | :heavy_check_mark: | Functional              |
| Formally Verified   | :x:                |                         |
| Exhaustively Tested | :x:                |                         |
| Production-Ready    | :heavy_check_mark: |                         |

## Feature Roadmap
- Benchmark performance vs other libraries
- Add variable size typedefs to reduce unnecessary memory usage
- Single bit error correction
- Multi bit error correction
- Formal verification of 1-bit CRC implementation
- Formal verification of full byte CRC implementation
- Support for 64+ bit CRCs
- Improve db_tests configurability with gtest

## Resolved
- ~~Automated releases CI setup~~
- ~~C++ library compatibility~~
- ~~Add convenience header with "common" CRCs~~
- ~~Implement basic fuzzer testing~~
- ~~Allow table precomputation with "advanced" APIs~~

# Dependencies

## Build Dependencies
* CMake >= 3.12
* C99 compliant compiler

## Dev Dependencies

* C++ compiler
* Python3
  * pip
  * requests
  * beautifulsoup4
* getopt
* [Verified Software Toolchain](https://vst.cs.princeton.edu/) (optional)
* clang-format (optional)
* clang-tidy (optional)

# Build Instructions

1. Create an output folder for an out-of-source build

2. `cd` to the output folder and invoke cmake with appropriate options.
    - Set `-DADDRESS_SANITIZER=1` or `-DUNDEFINED_SANITIZER=1` if appropriate to build with sanitizers
    - Set `-DENABLE_TESTING=0` if building tests is not desired
    - Set `-DENABLE_PEDANTIC_OPTS=0` if building with a compiler that does not support the following flags:
      - `-Wall -Werror -Wextra -Wno-unused-parameter -pedantic`

3. `make` (or other build system invocation)

# API Reference

### Macros
```c_cpp
DECLARE_CRC(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE, CHECK, WIDTH)
```
The `DECLARE_CRC()` macro will instantiate a CRC named `CRC_##NAME` according to
the input parameters. If `NAME` is "CCITT", the struct will be named "CRC_CCITT".

Parameters:
1. POLY
   - Must be `WIDTH` bits long, where 64 >= `WIDTH` >= 8.
   - Feedback polynomial for the CRC in standard form, with MSB omitted.
2. INIT
   - Initialization value for the data register.
3. REF_IN
   - true or false
   - If true, each input byte is reflected (inverted) before consumption.
4. REF_OUT
   - `true` or `false`
   - If `true`, each output word is reflected (inverted) before return.
5. XOR_OUT
   - Optional XOR mask applied to the output word.
   - Set to `0` if not needed.
6. RESIDUE
   - Optional
   - Specifies the remainder resulting from `CRC(message + CRC(message))`, where + is concatenation.
   - This value is not used by the library, but is useful for validating CRC'd messages.
7. CHECK
   - Optional
   - Specifies the remainder of `CRC("123456789")`.
   - This value is not used by the library, but is useful for sanity checking implementation correctness.
8. WIDTH
   - The width of the feedback polynomial in bits.
   - Constrained to 64 >= `WIDTH` >= 8 in the current implementation.

Many common CRCs are predefined and can be found in `<simplecrc/reference.h>`.

### Structs
```c_cpp
struct crc_def {
	uint64_t poly;
	uint64_t init;
	uint64_t xor_out;
	uint64_t residue;
	uint64_t check;
	uint64_t width;
	bool ref_in;
	bool ref_out;
};
```
This struct contains all the parameters necessary to define a CRC. It should be
set using the helper macro `DECLARE_CRC()`

### Functions

```c_cpp
uint64_t compute_crc(const unsigned char *buf, size_t len, struct crc_def params);
```
Calculate the CRC of a byte buffer `buf` of size `len`, according to the
algorithm defined by `params`. This function is not as performant as
compute_crc_fast (which omits potentially repeated table initializations), but
does not require managing memory manually.

```c_cpp
bool precompute_crc_table(struct crc_def params, uint64_t **table, size_t len);
```
Used in conjunction with compute_crc_fast(). Precomputes the CRC table using a
buffer supplied by the caller. The buffer must be of size `256*sizeof(uint64_t)`.
Returns `false` if called with a null pointer or if len is not correct. Returns
`true` if initialization succeeds. Caller must allocate and free the memory
associated with table.

```c_cpp
uint64_t compute_crc_fast(struct crc_def params, const unsigned char *buf, size_t len, const uint64_t *table);
```
Used in conjunction with precompute_crc_table(), which must be called first.
Computes the CRC of the byte buffer `buf` of size `len`, according to the
algorithm defined by `params. This function is functionally identical to
compute_crc() except that it takes the table initialized by
precompute_crc_table() as input and hence, avoids unnecessary table
initializations when computing the same CRC repeatedly. Caller must allocate and
free the memory associated with table.

### Example Usages
```c_cpp
#include <stdio.h>
#include <simplecrc.h>

int foo(void) {
    uint64_t result = 0;
    unsigned char buf[] = "123456789";
    DECLARE_CRC(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189, 16);
    result = compute_crc(buf, 9, CRC_KERMIT);
    printf("%s\n", (result == CRC_KERMIT.check) ? "PASS" : "FAIL");
}
```

```c_cpp
#include <stdio.h>
#include <simplecrc.h>
#include <simplecrc/reference.h>

int foo(void) {
    uint64_t crc = 0;
    unsigned char buf[] = "123456789";
    result = compute_crc(buf, 9, GSM_40);
    printf("%s\n", (result == CRC_GSM_40.check) ? "PASS" : "FAIL");
}
```

### Minimizing Runtime Footprint

If using `reference.h`, the following macros may be disabled to reduce static
memory usage. These macros are enabled by default and defining them will remove
the corresponding CRC definitions.

- INCLUDE_CRCS_8_15
- INCLUDE_CRCS_16_31
- INCLUDE_CRCS_16_31
- INCLUDE_CRCS_32_63
- INCLUDE_CRCS_MISC

# Useful Development Rules

1. `make format` to format code changes

2. `make check` or `ctest` to run tests

## Fuzzer Testing

SimpleCRC supports fuzzing with libFuzzer by setting the ENABLE_FUZZING flag
for CMake. Extended testing has thus far failed to discover any issues, but
feel free to explore for yourself.

# Implementation Details

SimpleCRC is implemented as a table based CRC that operates on 8 bit bytes.
The lookup table consumes 2048 bytes (256 entries * uint64_t) and is computed
prior to each CRC computation with a bit-by-bit algorithm. Macro magic was
originally available to select appropriately-sized integer types at compile time
to reduce memory overhead, but it was found to be problematic in reasonable use
cases. The current implementation uses a generic uint64_t type for most internal
operations to simplify validating correctness.

Once the lookup table is computed, the algorithm consumes the input byte-by-byte
until the input is exhausted. Consuming bytes individually provides a reasonable
balance between portability, speed, and implementation readability. Any lingering
ref_out or xor_out operations are computed once at the end before the remainder
is returned.
