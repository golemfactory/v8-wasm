V8 WASM code execution POC
=================================

This is a proof of concept of using the V8 interpreter to execute arbitrary
(possibly untrusted) WebAssembly code.

## Overview
This POC program converts the `example.crw` RAW image file to an `example.ppm` PPM image file
that can be viewed in most image viewers. The program `dcraw` compiled to WebAssembly with the
accompanying Javascript is used to do the conversion inside a V8 interpreter.

## Running
To run the code, you have to use Python >=3.6 and have [v8eval](https://github.com/sony/v8eval) installed.
Then, simply run `python main.py`.

## Remarks

* While the POC works correctly, moving data between Python code and V8 JS context proved to be extremely slow,
so this approach is abandoned.
* Due to the above, the script takes a couple minutes to run on a good computer.

## Files
* `dcraw.c` - the `dcraw` program source code used to produce the WASM and Javascript files
* `dcraw.wasm` - the result of compiling `dcraw.c`: WebAssembly code
* `dcraw.js` - the result of compiling `dcraw.c`: Emscripten glue and initiation JS code for the WASM file
* `example.crw` - an example input file for `dcraw`
* `example.ppm.ref` - reference output for `example.crw` produced by running regular `dcraw` on `example.crw`
* `example.ppm` - the result of running the POC script
* `main.py` - the POC script code