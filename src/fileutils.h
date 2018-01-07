//
// Created by justin on 31/12/17.
//

#ifndef AUTOSCP_FILEUTILS_H
#define AUTOSCP_FILEUTILS_H

#include <string>

struct FileUtils
{
    static std::string getParentPath(std::string);
    static std::string getEndPath(std::string);
    static std::string getRelativePath(std::string, std::string);
	static std::string toUnixPath(std::string);
    static std::string joinPath(std::string lhs, std::string rhs);
	static std::string joinPathLinux(std::string lhs, std::string rhs);
};


#endif //AUTOSCP_FILEUTILS_H
