//
// Created by justin on 25/12/17.
//

#ifndef AUTOSCP_FILEWATCHERLINUX_H
#define AUTOSCP_FILEWATCHERLINUX_H

#include <map>
#include "filewatcher.h"

class FileWatcherLinux : public FileWatcher
{
public:
    FileWatcherLinux(std::string _directory);

    void watch();
    void stop();

private:
    typedef std::map<int, std::string> dir_map_t;
    bool isDir(std::string );
    void addPath(std::string );
    int fd;
    dir_map_t wd_path;
    //std::vector<std::string> get_all_directories(std::string);
};

#endif //AUTOSCP_FILEWATCHERLINUX_H


