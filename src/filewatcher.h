//
// Created by justin on 25/12/17.
//

#ifndef AUTOSCP_FILEWATCHER_H
#define AUTOSCP_FILEWATCHER_H

#include <string>
#include <vector>

class FileWatcher {
public:
    FileWatcher(std::string);
    virtual bool start();
private:
    std::string directory;
    std::vector<std::string> modified;
};

#endif //AUTOSCP_FILEWATCHER_H
