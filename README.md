WebAssembly Sandbox
===================

## Overview

This is a proof of concept for a JavaScript + WebAssembly standalone sandbox implemented in `C++` utilizing the `V8` engine.

## Building

1. First of all, you have to build the V8 monolith library (`libv8_monolith.a`).
Instructions can be found [here](https://v8.dev/docs/embed#hello-world)
2. A compiler supporting C++17 along with the `filesystem` library has to be used for building, e.g. `g++-8`.
3. Inside the project root:
```
mkdir build
cd build
cmake .. \
    -DCMAKE_CXX_COMPILER={path to an appropriate compiler (see 2. above)} \
    -DV8_INCLUDE_DIR={path to the v8 include/ dir} \
    -DV8_MONOLITH_LIB_PATH={path to the compiled libv8_monolith.a from 1. above}
make
```
4. The above should leave you with a working `wasm-sandbox` executable.

## Running

You can run the program with the `--help` flag to obtain information about how to use it.