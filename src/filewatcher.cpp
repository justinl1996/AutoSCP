//
// Created by justin on 25/12/17.
//

#include "filewatcher.h"

FileWatcher::FileWatcher(std::string _directory) : directory(_directory)
{
    //printf("SSD\n");
}

std::vector<std::string> FileWatcher::getModified() const
{
    return modified;
}

std::vector<std::string> FileWatcher::getDeleted() const
{
    return deleted;
}