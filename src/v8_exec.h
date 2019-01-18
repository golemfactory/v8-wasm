#ifndef WASMCPP_V8EXEC_H
#define WASMCPP_V8EXEC_H

#include <exception>
#include <functional>
#include <libplatform/libplatform.h>
#include <string>
#include <string_view>
#include <v8.h>

/**
 * V8Executor manages embedding V8 and allows to execute arbitrary
 * JavaScript/WASM code and process its result.
 */
class V8Executor {
public:
    /**
     * ExecException represents an error occurring during
     * JavaScript/WASM execution inside V8.
     */
    class ExecException: public std::exception {
    public:
        explicit ExecException(std::string exec_exception_str);
        const char* what() const noexcept override;
    private:
        std::string exec_exception_str;
    };

    explicit V8Executor(std::string_view program_name);
    ~V8Executor();

    /**
     * Run the provided JavaScript code inside V8 and then process its result.
     *
     * @tparam T type of the result result_processor (and thus run_script) returns
     * @param src script code to run
     * @param result_processor function to process the result returned by V8
     *  (and possibly return something based on it)
     * @return the return value of the applied processor
     */
    template <typename T = void>
    T run_script(std::string_view src,
            std::function<T(v8::Local<v8::Value>)> result_processor = [](v8::Local<v8::Value>){});

private:
    void init_v8(std::string_view program_name);
    void set_new_isolate();
    void set_new_context();

    std::unique_ptr<v8::Platform> platform;
    v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> context;
    v8::Isolate* isolate;
    v8::ArrayBuffer::Allocator* isolate_allocator_ptr;
};

template <typename T>
T V8Executor::run_script(std::string_view src, std::function<T(v8::Local<v8::Value>)> result_processor) {
    v8::Locker locker{this->isolate};
    v8::Isolate::Scope isolate_scope{this->isolate};
    v8::HandleScope handle_scope{this->isolate};
    v8::Local<v8::Context> local_context = this->context.Get(this->isolate);
    v8::Context::Scope context_scope{local_context};

    v8::Local<v8::String> src_str = v8::String::NewFromUtf8(this->isolate, src.data(),
            v8::NewStringType::kNormal).ToLocalChecked();
    v8::MaybeLocal<v8::Script> script = v8::Script::Compile(local_context, src_str);
    if (script.IsEmpty()) {
        throw V8Executor::ExecException("syntax error");
    }

    v8::TryCatch trycatch{this->isolate};
    v8::MaybeLocal<v8::Value> result = script.ToLocalChecked()->Run(local_context);
    if (result.IsEmpty()) {
        v8::Local<v8::Value> exception = trycatch.Exception();
        v8::String::Utf8Value exception_str{this->isolate, exception};
        throw V8Executor::ExecException(
                std::string{*exception_str, static_cast<unsigned long>(exception_str.length())});
    }
    return result_processor(result.ToLocalChecked());
}

#endif //WASMCPP_V8EXEC_H
