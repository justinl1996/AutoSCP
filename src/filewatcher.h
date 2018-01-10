//
// Created by justin on 25/12/17.
//

#ifndef AUTOSCP_FILEWATCHER_H
#define AUTOSCP_FILEWATCHER_H

#include <string>
#include <vector>
#include <stack>

#include <boost/filesystem.hpp>
//#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

typedef std::pair<std::string, std::string> r_pair_t;

template <class T>
class VectorS : public std::vector<T>
{
public:
	
    bool empty() {
        shared.lock();
        bool ret = std::vector<T>::empty();
        shared.unlock();
        return ret;
    }
	T get_last(T def) {
		if (empty()) {
			return def;
		}
		return pop();
	}

    T pop() {
        shared.lock();
        T item = std::vector<T>::back();
        std::vector<T>::pop_back();
        shared.unlock();
        return item;
    };
    void push_back(T item) {
        shared.lock();
        std::vector<T>::push_back(item);
        shared.unlock();
    };
private:
    boost::shared_mutex shared;
	T def;
};

class FileWatcher {
public:

    FileWatcher(std::string);
    virtual void watch() = 0;
    virtual void stop() = 0;
    bool isIgnore(std::string);
    void clearAll();
    std::string getModified();
    std::string getDeleted();
    std::string getNewfile();
	std::string getNewDirectories();
    r_pair_t getRenamed();
    template<typename FILE_FUNC_T, typename DIRECTORY_FUNC_T> void
        traverse_directory(std::string, FILE_FUNC_T, DIRECTORY_FUNC_T);
    static std::string getRootPath(std::string);


protected:
    std::string full_path;
    //std::string root_path;
    VectorS<std::string> modified, deleted, new_files, new_directories;
    VectorS<r_pair_t> renamed;
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
