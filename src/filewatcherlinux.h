//
// Created by justin on 25/12/17.
//

#ifndef AUTOSCP_FILEWATCHERLINUX_H
#define AUTOSCP_FILEWATCHERLINUX_H
#include "filewatcher.h"

class FileWatcherLinux : public FileWatcher
{
public:
    FileWatcherLinux(std::string _directory);


    void watch();
    void stop();

private:
    int fd, wd;
    std::vector<std::string> get_all_directories(std::string);
};

#endif //AUTOSCP_FILEWATCHERLINUX_H


