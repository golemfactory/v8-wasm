#ifndef WASMCPP_EMSC_INTEGRATOR_H
#define WASMCPP_EMSC_INTEGRATOR_H

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <v8.h>
#include "files.h"
#include "v8_exec.h"

namespace fs = std::filesystem;

/**
 * EmscIntegrator integrates V8 with the glue JS code
 * produced by the Emscripten compiler.
 */
class EmscIntegrator {
public:
    /**
     * Exception represents an error happening during Emscripten integration.
     */
    class Exception: public std::exception {
    public:
        explicit Exception(const std::string& msg, const std::string& v8_internal_msg);
        const char* what() const noexcept override;
    private:
        std::string msg;
    };

    explicit EmscIntegrator(V8Executor& executor);
    EmscIntegrator(const V8Executor&& executor) = delete;

    /**
     * Sets the command line arguments for the program.

     * @param args the arguments to set
     */
    void set_exec_args(const std::vector<std::string>& args);

    /**
     * Map all of the files and directories inside the provided directory
     * to '/' in the Emscripten MemFS, giving the program access to them.
     *
     * The files should be available in MemFS at both '/file' and 'file'.
     *
     * @param input_files_dir path to the directory to be mapped
     */
    void load_input_files(const fs::path& input_files_dir);

    /**
     * Save the files which paths are in output_files from the Emscripten MemFS
     * to the actual filesystem inside output_dir. This can be used to retrieve
     * the program's results.
     *
     * Directory structure of the file paths is preserved, relative to output_dir.
     *
     * @param output_dir the path to the directory in which the files will be saved
     * @param output_files a list of file paths in MemFS to retrieve
     */
    void save_output_files(const fs::path& output_dir, const std::vector<fs::path>& output_files);

    /**
     * Run the Emscripten-compiled program.
     *
     * @param src_path path to the Emscripten-generated JavaScript
     *  initialization and glue file
     * @param wasm_path path to the Emscripten-generated WASM file
     */
    void run(std::string_view src_path, std::string_view wasm_path);

private:
    void init_integration();

    V8Executor& executor;
};

#endif //WASMCPP_EMSC_INTEGRATOR_H
