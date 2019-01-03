#include <filesystem>
#include <iostream>
#include <vector>
#include <tclap/CmdLine.h>
#include "emsc_integrator.h"
#include "v8_exec.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    std::string js_path, wasm_path, input_files_dir, output_files_dir;
    std::vector<std::string> exec_args, output_files;

    try {
        TCLAP::CmdLine cmd{"Untrusted JS & WASM code sandbox.", ' ' , "0.0.1"};
        TCLAP::ValueArg<std::string> js_path_arg{"j", "js", "JavaScript source file path.", true, "", "string", cmd};
        TCLAP::ValueArg<std::string> wasm_path_arg{"w", "wasm", "WebAssembly source file path.", true, "", "string", cmd};
        TCLAP::ValueArg<std::string> input_files_dir_arg{"I", "input-files-dir", "The input files directory path.", true, "", "string", cmd};
        TCLAP::ValueArg<std::string> output_files_dir_arg{"O", "output-files-dir", "The output files directory path.", true, "", "string", cmd};
        TCLAP::MultiArg<std::string> output_files_arg{"o", "output-file", "A path to an expected output file.", false, "string", cmd};
        TCLAP::UnlabeledMultiArg<std::string> exec_args_arg{"exec-arg", "Command line argument passed to the sandboxed program.", false, "string", cmd, true};
        cmd.parse(argc, argv);
        js_path = js_path_arg.getValue();
        wasm_path = wasm_path_arg.getValue();
        input_files_dir = input_files_dir_arg.getValue();
        output_files_dir = output_files_dir_arg.getValue();
        output_files = output_files_arg.getValue();
        exec_args = exec_args_arg.getValue();
    }
    catch (TCLAP::ArgException& e) {
        std::cerr << "Argument parsing error: " << e.error() << " for arg: " << e.argId() << "!" << std::endl;
    }

    std::vector<fs::path> output_files_paths(output_files.begin(), output_files.end());

    V8Executor executor{argv[0]};
    EmscIntegrator integrator{executor};

    integrator.set_exec_args(exec_args);
    integrator.load_input_files(input_files_dir);
    integrator.run(js_path, wasm_path);
    integrator.save_output_files(output_files_dir, output_files_paths);

    return 0;
}