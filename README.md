# v8-based WebAssembly Sandbox

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

Example:
```bash
./wasm-sandbox -O output/ -I input/ -j program.js -w program.wasm\
    -o output_file1 -o output_file2 -- program_arg1 --program_flag1 program_arg2
```

### Flags:

* `-O` : Path to the output directory. All files specified by `-o` will be copied from the program
 virtual filesystem to this directory.
* `-I` : Path to the input directory. The contents of this directory will be mapped as `/` in the
program's virtual filesystem (which is also the *CWD* of the program).
* `-w` : Path to the WebAssembly program file produced by the compiler (e.g. Emscripten).
* `-j` : Path to the JavaScript program file produced by the compiler (e.g. Emscripten).
* `-o` [**repeated**] : Path to an output file produced by the program inside its virtual filesystem.
Each file specified by this flag will be copied to the directory specified by `-O` after the program
finishes. If any of the files specified by `-o` cannot be found after the program finishes, an exception
will be raised.
* `--` : Anything after this will be passed to the program being run as arguments.

### Program restrictions

Currently only programs compiled by *Emscripten* are supported. Furthermore, the following flags
**have** to be used while compiling via *emcc*:

* `-s BINARYEN_ASYNC_COMPILATION=0` - without this, the program will not actually run, but it
won't show any errors (quite difficult to debug). This is due to the fact that the sandbox
doesn't implement any sort of asynchronicity on top of V8, so the WebAssembly compilation has
to be done synchronously.
* `-s MEMFS_APPEND_TO_TYPED_ARRAYS=1` - without this, writing bigger (the actual size seems
to depend on the program) files will fail with OOM errors.

If the program uses any substantial amount of memory, the following flags may also be useful:

* `-s ALLOW_MEMORY_GROWTH=1` 
* `-s TOTAL_MEMORY=1073741824` (or another number, though there are restrictions). It seems to
sometimes be required to specify this flag even though `-s ALLOW_MEMORY_GROWTH=1` is specified
as well (see the [flite](examples/flite) example).

### Example:

Let's say `test.c` takes a string argument, reads `in.txt` file and writes the following
string: `Hello world!\n{contents of the input file}\n{the argument}\n` to `out.txt` file.

First, we compile the program using *Emscripten*:

```bash
emcc test.c -o test.js -s BINARYEN_ASYNC_COMPILATION=0 -s MEMFS_APPEND_TO_TYPED_ARRAYS=1 -s ALLOW_MEMORY_GROWTH=1
```

This produces 2 files, `test.js` and `test.wasm`. Then, we create directories for the program's
input and output:

```bash
mkdir input output
echo "interesting file contents" > input/in.txt
```

Finally, we can run the program inside the sandbox:

```bash
./wasm-sandbox -I input -O output -j test.js -w test.wasm -o out.txt -- interesting_program_arg
```

And the we can read the results:

```bash
cat output/out.txt
> Hello world!
> interesting file contents
> interesting_program_arg
```

The actual program can be found in [examples/test](examples/test), both as source and compiled into JS+WASM.

## Wasm store

More examples of precompiled Wasm binaries can be found in [golemfactory/wasm-store](https://github.com/golemfactory/wasm-store) repo.

## Possible TODOs

* Allow outputing variable number of files (perhaps whole directories?).
* Sync to a newer release of V8.
* Figure out if there is a more performant way of linking V8 than `libv8_monolith.a`.
* Create an abstract *Integrator* interface, which *EmscriptenIntegrator* will implement, if any
different integrators are needed in the future, targeting different compilers than *Emscripten*.
