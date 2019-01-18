#ifndef WASMCPP_V8_CALLBACKS_H
#define WASMCPP_V8_CALLBACKS_H

#include <string_view>
#include <v8.h>

/**
 * V8Callbacks holds callback functions that are bound to their
 * corresponding JavaScript global functions. V8 requires the callbacks
 * to be static.
 */
struct V8Callbacks {
    /**
     * readCallback allows JS code to read files from VirtualFS.
     * JS use: read(path) for text files, read(path, 'binary') for binary files.
     */
    static void readCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    /**
     * printCallback allows JS code to print values to stdout.
     * JS use: print(val1, val2, ...).
     */
    static void printCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

private:
    static void throw_exception(const v8::FunctionCallbackInfo<v8::Value>& args, std::string_view exc_msg);
};

#endif //WASMCPP_V8_CALLBACKS_H
