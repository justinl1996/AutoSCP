//
// Created by justin on 25/12/17.
//

#include <stack>
#include <boost/filesystem.hpp>
#include "filewatcher.h"

FileWatcher::FileWatcher(std::string _directory) : directory(_directory)
{
    typedef std::vector<std::string> vec_str_t;

    vec_str_t directories;
    vec_str_t files;
    auto file_process = [&](std::string path) {files.push_back(path);};
    auto directory_process = [&](std::string path) {directories.push_back(path);};
    traverse_directory(directory, file_process, directory_process);

    for(std::string file : files) {
        std::cout << file << std::endl;
    }
}

std::vector<std::string> FileWatcher::getModified() const
{
    return modified;
}

std::vector<std::string> FileWatcher::getDeleted() const
{
    return deleted;
}

