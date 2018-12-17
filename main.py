from v8eval import V8, set_flags


def main():
    set_flags('--max-old-space-size=8192')
    v8 = V8()

    with open('dcraw.wasm', 'rb') as f_wasm, open('example.crw', 'rb') as f_data:
        v8.eval(f'var wasmBinary = Uint8Array.from({str(list(f_wasm.read()))});')
        v8.eval(f'var inputData = Uint8Array.from({str(list(f_data.read()))});')

    v8.eval(
        """
        var Module = {
            'wasmBinary': wasmBinary,
            'arguments': ['/example.crw'],
            'preRun': function() {
                Module['FS_createDataFile']('/', 'example.crw', inputData, true, true);
            },
        };
        """
    )

    with open('dcraw.js', 'r') as f_js:
        v8.eval(f_js.read())

    output_file_data = v8.eval('FS.readFile("/example.ppm")')
    with open('example.ppm', 'wb') as f:
        f.write(bytearray(output_file_data.values()))


if __name__ == '__main__':
    main()
