//
// Created by justin on 25/12/17.
//

#include <stack>
#include <boost/filesystem.hpp>
#include "filewatcher.h"


FileWatcher::FileWatcher(std::string _path) : full_path(_path)
{
    //using namespace boost::filesystem;


    /*typedef std::vector<std::string> vec_str_t;

    vec_str_t directories;
    vec_str_t files;

    auto file_process = [&](std::string path) {files.push_back(path);};
    auto directory_process = [&](std::string path) {
        directories.push_back(path);
        std::cout << path << std::endl;
    };
    traverse_directory(full_path, file_process, directory_process);
    */
    /*for(std::string file : files) {
        std::cout << file << std::endl;
    }*/
}
std::string FileWatcher::getRootPath(std::string path)
{
    boost::filesystem::path p(path);
    //std::cout << p.stem() << std::endl;
    return p.root_path().string();
}

void FileWatcher::clearAll()
{
    new_files.clear();
    modified.clear();
    deleted.clear();
	new_directories.clear();
    //std::cout << new_files.size() << std::endl;
}

//todo: allow user to input directory + files to ignore
bool FileWatcher::isIgnore(std::string file)
{
    //just ignore hidden files for now
    return file.at(0) == '.' || *(file.end() - 1) == '~' ? true : false;
}

std::string FileWatcher::getNewfile()
{
	return new_files.get_last("");
}

std::string FileWatcher::getNewDirectories()
{
	return new_directories.get_last("");
}

std::string FileWatcher::getModified()
{
	return modified.get_last("");
}

std::string FileWatcher::getDeleted()
{
	return deleted.get_last("");
}

r_pair_t FileWatcher::getRenamed()
{
	return renamed.get_last(std::make_pair(std::string(""), std::string("")));
}

