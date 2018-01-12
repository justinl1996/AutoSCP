//
// Created by justin on 30/12/17.
//

#ifndef AUTOSCP_FILEMANAGER_H
#define AUTOSCP_FILEMANAGER_H
#include <iostream>
#include <vector>
#include "scpmanager.h"
#include "filewatcher.h"


class FileManager
{

public:
    FileManager(std::string, std::string, SCPManager::ssh_ptr_t ssh);
    //~FileManager() {std::cout << "IN DESTRUCTOR\n";};
	bool checkRemoteDir() { return scp->directoryExists(dest); };
	void start();
    void syncAll();

private:

    std::string source, dest;
    std::unique_ptr<FileWatcher> filewatch;
    std::unique_ptr<SCPManager> scp;

};

#endif //AUTOSCP_FILEMANAGER_H
