//
// Created by justin on 25/12/17.
//

#ifndef AUTOSCP_FILEWATCHER_H
#define AUTOSCP_FILEWATCHER_H

#include <string>
#include <vector>
#include <stack>
#include <boost/filesystem.hpp>


class FileWatcher {
public:
    FileWatcher(std::string);
    virtual void watch() = 0;
    virtual void stop() = 0;
    std::vector<std::string> getModified() const;
    std::vector<std::string> getDeleted() const;
    template<typename FILE_FUNC_T, typename DIRECTORY_FUNC_T> void
        traverse_directory(std::string, FILE_FUNC_T, DIRECTORY_FUNC_T);

protected:
    std::string directory;
    std::vector<std::string> modified;
    std::vector<std::string> deleted;
};

template<typename FILE_FUNC_T, typename DIRECTORY_FUNC_T>
void FileWatcher::traverse_directory(std::string start,
    FILE_FUNC_T file_func, DIRECTORY_FUNC_T dir_func)
{
    using namespace boost::filesystem;
    std::stack<std::string> st;
    if (!exists(start))
    {
        throw std::runtime_error("No such directory exist: " + start);
    }

    st.push(start);

    while(!st.empty())
    {
        path p = st.top();
        st.pop();

        if (is_directory(p))
        {
            dir_func(p.string());
            directory_iterator end;
            for( directory_iterator iter(p) ; iter != end ; ++iter ) {
                st.push(iter->path().string());
            }
        } else {
            file_func(p.string());
        }
    }
}

#endif //AUTOSCP_FILEWATCHER_H
