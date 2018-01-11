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

#ifndef _WIN32
struct stat FileUtils::getFileStat(std::string path)
{
    struct stat buf;
    stat(path.c_str(), &buf);
    return buf;
}

std::time_t FileUtils::getLastModified(std::string file)
{
   return FileUtils::getFileStat(file).st_mtim.tv_sec;
}


#endif



mode_t FileUtils::getFilePermissions(std::string file)
{
#if _WIN32
	return 0x180; //500
#else
    FileUtils::getFileStat(file).st_mode;
#endif
}

mode_t FileUtils::getDirectoryPermissions(std::string file)
{
#if _WIN32
	return 0x1c0; //700
#else
	//return getFilePermissions(file);
    return 0x1c0;
#endif
}
