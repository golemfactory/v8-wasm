#include <iostream>
#include <sstream>
#include "emsc_integrator.h"

EmscIntegrator::Exception::Exception(const std::string& msg, const std::string& v8_internal_msg)
        : msg{msg + "\nInternal V8 error:\n" + v8_internal_msg} {}

const char* EmscIntegrator::Exception::what() const noexcept {
    return this->msg.c_str();
}

EmscIntegrator::EmscIntegrator(V8Executor& executor)
        : executor{executor} {
    this->init_integration();
}

void EmscIntegrator::init_integration() {
    try {
        this->executor.run_script("var Module = {'printErr': print, 'print': print};\n");
    }
    catch (V8Executor::ExecException& e) {
        throw EmscIntegrator::Exception("Integration initialization code failed.", e.what());
    }
}

void EmscIntegrator::set_exec_args(const std::vector<std::string>& args) {
    std::stringstream args_set_ss;

    args_set_ss << "Module['arguments'] = [";
    for (auto const& arg : args) {
        args_set_ss << "'" << arg << "', ";
    }
    args_set_ss << "];\n";

    try {
        this->executor.run_script(args_set_ss.str());
    }
    catch (V8Executor::ExecException& e) {
        throw EmscIntegrator::Exception("Execution argument setting failed.", e.what());
    }
}

void EmscIntegrator::run(std::string_view src_path, std::string_view wasm_path) {
    // TODO: Change it to an UUID?
    const char WASM_FILE_NAME[] = "___WASM_CODE___.wasm";
    VirtualFS::singleton().add_file(wasm_path, WASM_FILE_NAME);
    std::stringstream wasm_load_ss;
    wasm_load_ss << "Module['wasmBinary'] = read('" << WASM_FILE_NAME << "', 'binary');\n";
    try {
        this->executor.run_script(wasm_load_ss.str());
    }
    catch (V8Executor::ExecException& e) {
        throw EmscIntegrator::Exception("Loading the WASM binary failed.", e.what());
    }

    auto src_vec = FileManager::read_file(src_path);
    // Cast to a string because FileManager::read_file doesn't null-terminate.
    std::string src{src_vec.data(), src_vec.size()};
    try {
        this->executor.run_script(src.data());
    }
    catch (V8Executor::ExecException& e) {
        throw EmscIntegrator::Exception("Running the program failed.", e.what());
    }
}

void EmscIntegrator::load_input_files(const fs::path& input_files_dir) {
    std::stringstream file_create_ss;
    file_create_ss << "Module['preRun'] = function() {\n";

    FileManager::DirAnalysis dir_analysis = FileManager::get_dir_analysis(input_files_dir);

    for (auto const& [dir, dir_relative]: dir_analysis.dirs) {
        file_create_ss << "\tFS.mkdir('/" << dir_relative << "');\n";
    }
    for (auto const& [file, file_relative]: dir_analysis.files) {
        VirtualFS::singleton().add_file(file, file_relative);
        file_create_ss << "\tFS.writeFile('/" << file_relative
                       << "', new Uint8Array(read('"<< file_relative << "', 'binary')));\n";;
    }

    file_create_ss << "};\n";
    try {
        this->executor.run_script(file_create_ss.str());
    }
    catch (V8Executor::ExecException& e) {
        throw EmscIntegrator::Exception("Loading input files failed.", e.what());
    }
}

void EmscIntegrator::save_output_files(const fs::path& output_dir, const std::vector<fs::path>& output_files) {
    for(auto const& path : output_files) {
        std::stringstream read_script_ss;
        // MemFS uses '/' as separator, so generic_string() has to be used bere.
        read_script_ss << "FS.readFile('/" << path.generic_string() << "');";

        try {
            this->executor.run_script<void>(read_script_ss.str(), [&output_dir, &path] (v8::Local<v8::Value> val) {
                v8::ArrayBuffer::Contents buf = val.As<v8::Uint8Array>()->Buffer()->GetContents();
                FileManager::write_file_to_dir(output_dir, path,
                                               std::string_view(reinterpret_cast<const char *>(buf.Data()), buf.ByteLength()));
            });
        }
        catch (V8Executor::ExecException& e) {
            throw EmscIntegrator::Exception("Saving output files failed.", e.what());
        }
    }
}
