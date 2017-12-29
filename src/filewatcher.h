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
    virtual void watch() = 0;
    virtual void stop() = 0;
    std::vector<std::string> getModified() const;
    std::vector<std::string> getDeleted() const;

protected:
    std::string directory;
    std::vector<std::string> modified;
    std::vector<std::string> deleted;
};

#endif //AUTOSCP_FILEWATCHER_H
