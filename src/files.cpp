#include <fstream>
#include <sstream>
#include "files.h"


FileManager::Exception::Exception(std::string msg) : msg{std::move(msg)} {}

const char* FileManager::Exception::what() const noexcept {
    return this->msg.c_str();
}

std::vector<char> FileManager::read_file(const fs::path& path) {
    std::vector<char> result;
    std::ifstream f{path};
    f.seekg(0, std::ios::end);
    long size = f.tellg();
    if (size < 0) {
        std::stringstream exc_msg_ss;
        exc_msg_ss << "error while opening file '" << path.string() << "'";
        throw FileManager::Exception(exc_msg_ss.str());
    }
    result.resize(static_cast<std::size_t>(size));
    f.seekg(0, std::ios::beg);
    f.read(result.data(), result.size());
    return result;
}

void FileManager::write_file_to_dir(const fs::path& dir_path, const fs::path& file_path, std::string_view content) {
    fs::path dir_path_normal = dir_path.lexically_normal();
    fs::path final_file_path_normal = (dir_path / file_path).lexically_normal();

    // Ensure that the final file path doesn't point outside of the directory path.
    // TODO: The below is probably not enough, revise.
    std::string dir_path_normal_str = dir_path_normal.string();
    if (final_file_path_normal.string().compare(0, dir_path_normal_str.length(), dir_path_normal_str) != 0) {
        std::stringstream exc_msg_ss;
        exc_msg_ss << "path (" << file_path << ") would not end up in directory ("
                   << dir_path << "): possible path traversal attack";
        throw FileManager::Exception(exc_msg_ss.str());
    }

    fs::create_directories(final_file_path_normal.parent_path());
    std::ofstream f_out{final_file_path_normal};
    f_out.write(content.data(), content.length());
}

FileManager::DirAnalysis FileManager::get_dir_analysis(const fs::path& dir_path) {
    fs::recursive_directory_iterator iter;
    try {
        iter = fs::recursive_directory_iterator(dir_path);
    }
    catch (fs::filesystem_error& e) {
        std::stringstream exc_msg_ss;
        exc_msg_ss << "error while analyzing directory '" << dir_path.string() << "':\n\t" << e.what();
        throw FileManager::Exception(exc_msg_ss.str());
    }

    DirAnalysis dir_analysis;

    for(const auto& p: fs::recursive_directory_iterator(dir_path)) {
        const fs::path& path = p.path();
        const fs::path& relative_path = fs::relative(path, dir_path);

        if (p.is_regular_file()) {
            dir_analysis.files.emplace_back(path.string(), relative_path.string());
        }
        else if (p.is_directory()) {
            dir_analysis.dirs.emplace_back(path.string(), relative_path.string());
        }
    }
    return dir_analysis;
}

void VirtualFS::add_file(const fs::path& src_path, std::string dst_path) {
    this->files.insert_or_assign(std::move(dst_path), FileManager::read_file(src_path));
}

bool VirtualFS::file_exists(std::string_view path) {
    return this->files.count(path.data()) == 1;
}

std::vector<char>& VirtualFS::get_file_content(std::string_view path) {
    return this->files.at(path.data());
}
