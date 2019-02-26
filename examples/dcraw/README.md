dcraw
=======

A program to convert RAW photo files to viewable *\*.ppm* files.

* [Homepage](https://www.cybercom.net/~dcoffin/dcraw/)
* [Source file](https://www.cybercom.net/~dcoffin/dcraw/dcraw.c)

## Compilation

Compiles without issues, after adding the following flags to *emcc*:
```
-lm -DNODEPS
```

**NOTE:** The NODEPS compilation flag disables the following features in *dcraw*
(but makes it easier to compile):
* Decoding Red camera movies.
* Decoding compressed Kodak DC120 photos and Adobe Lossy DNGs.
* Support for color profiles.

## Invocation

```bash
./wasm-sandbox -I example_input -O example_output -j dcraw.js\
    -w dcraw.wasm -o example.ppm -- example.crw
```
