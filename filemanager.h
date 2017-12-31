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

private:

    std::string source, dest;
    std::unique_ptr<FileWatcher> filewatch;
    SCPManager &scp;

};

#endif //AUTOSCP_FILEMANAGER_H
