#include <iostream>
#include <vector>
#include "files.h"
#include "v8_callbacks.h"

void V8Callbacks::readCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope{args.GetIsolate()};

    if (args.Length() != 1 && args.Length() != 2) {
        return throw_exception(args, "read: wrong parameter number");
    }

    bool binary = false;
    if (args.Length() == 2) {
        v8::String::Utf8Value mode_val{args.GetIsolate(), args[1]};
        if (std::string{*mode_val} != "binary") {
            return throw_exception(args, "read: incorrect mode");
        }
        binary = true;
    }

    v8::String::Utf8Value path_val{args.GetIsolate(), args[0]};
    std::string path_str{*path_val};

    if (!VirtualFS::singleton().file_exists(path_str)) {
        return throw_exception(args, "read: file not found (" + path_str + ")");
    }
    std::vector<char>& content = VirtualFS::singleton().get_file_content(path_str);

    if (binary) {
        v8::Local<v8::ArrayBuffer> buf = v8::ArrayBuffer::New(args.GetIsolate(), content.data(), content.size());
        args.GetReturnValue().Set(buf);
        return;
    }

    v8::MaybeLocal<v8::String> maybe_str = v8::String::NewFromUtf8(args.GetIsolate(),
            content.data(), v8::NewStringType::kNormal);
    if (maybe_str.IsEmpty()) {
        return throw_exception(args, "read: file isn't valid utf8");
    }
    args.GetReturnValue().Set(maybe_str.ToLocalChecked());
}

void V8Callbacks::printCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope{args.GetIsolate()};
        if (i != 0) {
            printf(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        printf("%s", *str ? *str : "<failed str conversion>");
    }
    printf("\n");
}

void V8Callbacks::throw_exception(const v8::FunctionCallbackInfo<v8::Value>& args, std::string_view exc_msg) {
    v8::HandleScope handle_scope{args.GetIsolate()};
    v8::Local<v8::String> exc_str = v8::String::NewFromUtf8(args.GetIsolate(),
            exc_msg.data(), v8::NewStringType::kNormal).ToLocalChecked();
    args.GetIsolate()->ThrowException(exc_str);
}
