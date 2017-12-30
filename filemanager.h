//
// Created by justin on 30/12/17.
//

#ifndef AUTOSCP_FILEMANAGER_H
#define AUTOSCP_FILEMANAGER_H
#include <iostream>
#include "scpmanager.h"
#include "filewatcher.h"


class FileManager
{

public:
    FileManager(std::string, std::string, SCPManager &);
    //~FileManager() {std::cout << "IN DESTRUCTOR\n";};
    void start();
    void syncAll();

    //todo: put static methods into their own class
    static std::string getParentPath(std::string);
    static std::string getEndPath(std::string);
    static std::string getRelativePath(std::string, std::string);
private:

    std::string source, dest;
    std::unique_ptr<FileWatcher> filewatch;
    SCPManager &scp;

};

#endif //AUTOSCP_FILEMANAGER_H
