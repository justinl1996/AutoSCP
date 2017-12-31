//
// Created by justin on 31/12/17.
//

#include "fileutils.h"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

std::string FileUtils::getParentPath(std::string filepath)
{
    path p(filepath);
    return p.parent_path().string();
}

std::string FileUtils::getEndPath(std::string filepath)
{
    path p(filepath);
    return p.stem().string();
}

std::string FileUtils::getRelativePath(std::string filepath, std::string root)
{
    return filepath.substr(root.size() + 1, filepath.length());
}

std::string FileUtils::joinPath(std::string lhs, std::string rhs)
{
    return (path(lhs) / path(rhs)).make_preferred().string();
}