#ifndef WASMCPP_FILES_H
#define WASMCPP_FILES_H

#include <exception>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

/**
 * FileManager consists of utility functions to interact
 * with the actual host filesystem.
 */
struct FileManager {
    /**
     * Exception represents an error happening during filesystem interaction.
     */
    class Exception: public std::exception {
    public:
        explicit Exception(std::string msg);
        const char* what() const noexcept override;
    private:
        std::string msg;
    };

    /**
     * Read a file from a filesystem. File is read as binary.
     *
     * @param path path to the file in the filesystem
     * @return the file contents
     */
    static std::vector<char> read_file(const fs::path& path);

    /**
     * Write content to a file inside a directory. The path to the file must
     * be relative to the directory. If it isn't, an exception is thrown, indicating
     * a possible path traversal attack.
     *
     * This can be used to ensure that a untrusted file_path doesn't try to
     * point outside of the trusted dir_path after concatenation.
     *
     * Creates any missing directories on the way from dir_path to file_path (including
     * the outmost dir_path directory, if it doesn't exist).
     *
     * @param dir_path path to the directory
     * @param file_path path to the file (relative to the directory)
     * @param content content to write to the file
     */
    static void write_file_to_dir(const fs::path& dir_path, const fs::path& file_path, std::string_view content);

    /**
     * Contains recursive information about a directory.
     */
    struct DirAnalysis {
        /**
         * A recursive list of all regular files inside the directory.
         * Each entry is a pair of paths: the first including the directory
         * at the beginning and the second relative to the directory.
         */
        std::vector<std::pair<std::string, std::string>> files;

        /**
         * A recursive list of all directories inside the directory.
         * Each entry is a pair of paths: the first including the directory
         * at the beginning and the second relative to the directory.
         */
        std::vector<std::pair<std::string, std::string>> dirs;
    };

    /**
     * Returns an analysis of the given directory.
     *
     * @param dir_path path to the directory
     * @return the directory's analysis
     */
    static DirAnalysis get_dir_analysis(const fs::path& dir_path);
};

/**
 * VirtualFS is a key-value store that's supposed to emulate a
 * (very) simple filesystem in memory.
 */
class VirtualFS {
public:
    /**
     * Return a singleton instance of this class. This is needed,
     * because the V8 callbacks that have to access the virtual file
     * system are static, so they need to have a static instance of this
     * class.
     * @return
     */
    static VirtualFS& singleton() {
        // This has to be a function to avoid static initialization order fiasco.
        static VirtualFS _singleton;
        return _singleton;
    }

    /**
     * Add a file from path src_path in the actual filesystem to the
     * virtual filesystem at path dst_path.
     *
     * @param src_path the path to the source file in the actual filesystem
     * @param dst_path the path the resulting file is supposed to have in
     *  the virtual filesystem
     */
    void add_file(const fs::path& src_path, std::string dst_path);

    /**
     * Check if a file at a given path exists in the virtual filesystem.
     *
     * @param path path of the file to check
     * @return is the file in the filesystem
     */
    bool file_exists(std::string_view path);

    /**
     * Access the contents of the file at a given path in the virtual
     * filesystem.
     *
     * @param path to the file
     * @return the contents of the file
     */
    std::vector<char>& get_file_content(std::string_view path);

private:
    std::unordered_map<std::string, std::vector<char>> files;
};

#endif //WASMCPP_FILES_H
