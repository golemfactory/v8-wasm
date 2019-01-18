#include "v8_callbacks.h"
#include "v8_exec.h"

V8Executor::ExecException::ExecException(std::string exec_exception_str)
        : exec_exception_str{std::move(exec_exception_str)} {}

const char* V8Executor::ExecException::what() const noexcept {
    return this->exec_exception_str.c_str();
}

V8Executor::V8Executor(std::string_view program_name)
        : isolate{nullptr}, isolate_allocator_ptr{nullptr} {
    this->init_v8(program_name);
    this->set_new_isolate();
    this->set_new_context();
}

V8Executor::~V8Executor() {
    if (!this->context.IsEmpty()) {
        this->context.Reset();
    }

    if (this->isolate) {
        this->isolate->Dispose();
        delete isolate_allocator_ptr;
    }

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}

void V8Executor::init_v8(std::string_view program_name) {
    v8::V8::InitializeICUDefaultLocation(program_name.data());
    v8::V8::InitializeExternalStartupData(program_name.data());
    this->platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
}

void V8Executor::set_new_isolate() {
    v8::Isolate::CreateParams create_params;
    this->isolate_allocator_ptr = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    create_params.array_buffer_allocator = this->isolate_allocator_ptr;
    this->isolate = v8::Isolate::New(create_params);
}

void V8Executor::set_new_context() {
    v8::Locker locker{this->isolate};
    v8::Isolate::Scope isolate_scope{this->isolate};
    v8::HandleScope handle_scope{this->isolate};

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(this->isolate);
    global->Set(v8::String::NewFromUtf8(this->isolate, "print"),
            v8::FunctionTemplate::New(this->isolate, V8Callbacks::printCallback));
    global->Set(v8::String::NewFromUtf8(this->isolate, "read"),
            v8::FunctionTemplate::New(this->isolate, V8Callbacks::readCallback));

    v8::Local<v8::Context> context = v8::Context::New(this->isolate, nullptr, global);
    this->context = v8::Persistent<v8::Context>{this->isolate, context};
}
