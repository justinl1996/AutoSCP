//
// Created by justin on 25/12/17.
//

#include "filewatcherlinux.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <iostream>
#include <dirent.h>
#include <stack>
#include <sys/inotify.h>
#include <unistd.h>

static const int EVENT_SIZE =  ( sizeof (struct inotify_event) );
static const int BUF_LEN = ( 1024 * ( EVENT_SIZE + 9 ) );

FileWatcherLinux::FileWatcherLinux(std::string directory) :
        FileWatcher(directory) {
    fd = inotify_init();

    if (fd < 0) {
        perror("inotify_init");
    }

    typedef std::vector<std::string> vec_str_t;

    vec_str_t directories;
    vec_str_t files;

    auto file_process = [&](std::string path) {files.push_back(path);};
    auto directory_process = [&](std::string path) {directories.push_back(path);};
    traverse_directory(directory, file_process, directory_process);
    /*auto file_process = [=](std::string filename) {std::cout << filename << std::endl;};
    std::vector<std::string> vec = get_all_directories(directory, file_process);
    for (std::string path: vec) {
        //std::cout << path << std::endl;
        int wd = inotify_add_watch(fd, path.c_str(),
                                   IN_MODIFY | IN_CREATE | IN_DELETE);
        wd_path[wd] = path;
    }*/
}


void FileWatcherLinux::watch()
{
    int length, i = 0;
    char buffer[BUF_LEN];

    length = read( fd, buffer, BUF_LEN );

    if ( length < 0 ) {
        perror( "read" );
    }

    while ( i < length ) {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        if ( event->len ) {
            std::string path = wd_path[event->wd] + "/" + event->name;
            if ( event->mask & IN_CREATE || event->mask & IN_MODIFY ) {
                modified.push_back(path);
            }
            else if ( event->mask & IN_DELETE ) {
                deleted.push_back(path);
            }
        }
        //printf("size: %ld\n", event->len);
        i += EVENT_SIZE + event->len;
    }
}


/*std::vector<std::string> FileWatcherLinux::get_all_directories(std::string start)
{
    std::vector<std::string> directories;
    std::stack<std::string> st;

    st.push(start);
    directories.push_back(start);

    while(!st.empty()) {
        std::string directory = st.top();
        st.pop();

        DIR *d = opendir(directory.c_str());
        struct dirent * entry;

        while (1) {
            entry = readdir(d);
            if (entry == NULL)
                break;

            std::string path = entry->d_name;
            if (entry->d_type & DT_DIR) {
                if (!(path == "." or path == "..")) {
                    //printf("%s/%s\n", directory.c_str(), path.c_str());
                    path = directory + "/" + path;
                    st.push(path);
                    directories.push_back(path);

                }
            }
            //printf("%s\n", path.c_str());
        }
    }

    return directories;
}*/

void FileWatcherLinux::stop()
{
   //( void ) inotify_rm_watch( fd, wd );
   for (dir_map_t::iterator p = wd_path.begin(); p != wd_path.end(); p++)
   {
       (void) inotify_rm_watch(fd, p->first);
   }

   ( void ) close( fd );
}