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
	const int i = 1;

    return filepath.substr(root.length() + i, filepath.length());
}

std::string FileUtils::toUnixPath(std::string path)
{
	do {
		auto found = path.find("\\");
		if (found == std::string::npos) {
			break;
		}
		path.replace(found, 1, "/");
	} while (1);
	return path;
}

std::string FileUtils::joinPath(std::string lhs, std::string rhs)
{
    return (path(lhs) / path(rhs)).make_preferred().string();
}

std::string  FileUtils::joinPathLinux(std::string lhs, std::string rhs)
{
	return lhs == "" ? rhs : lhs + "/" + rhs;
}