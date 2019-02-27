flite
======

A program to convert text to speech (*TTS*).

* [Homepage](http://www.festvox.org/flite/)
* [Source code](http://www.festvox.org/flite/packed/flite-1.4/flite-1.4-release.tar.bz2)

## Compilation

```bash
emconfigure ./configure
emmake make
cd bin
mv flite flite.bc
emcc flite.bc -o flite.js -s BINARYEN_ASYNC_COMPILATION=0\
 -s TOTAL_MEMORY=1073741824 -s ALLOW_MEMORY_GROWTH=1\
 -s MEMFS_APPEND_TO_TYPED_ARRAYS=1
```

**NOTE**: The `-s TOTAL_MEMORY=1073741824` flag above is important, even though
`-s ALLOW_MEMORY_GROWTH=1` is specified as well. Without it, the compilation fails (go figure).

## Invocation

```bash
./wasm-sandbox -I example_input -O example_output -j flite.js\
    -w flite.wasm -o intro.wav -- intro.txt intro.wav
```