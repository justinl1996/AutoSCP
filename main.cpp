#include <iostream>
#include <vector>

/*
struct AddOne{
    int foo(int i) {
        return i + 1;
    }
};

template <typename func_T>
int add_one(std::vector<int> expand, func_T func)
{
    typedef std::vector<int>::iterator vec_t;

    for (vec_t p = expand.begin(); p != expand.end(); p++) {
        printf("%d\n", func.foo(*p));
        //printf("%d\n", *p);
    }
    return 0;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 9 ) )

int main( int argc, char **argv )
{
    int length, i = 0;

    int fd;
    int wd;
    char buffer[BUF_LEN];

    fd = inotify_init();

    if ( fd < 0 ) {
        perror( "inotify_init" );
    }

    wd = inotify_add_watch( fd, "/home/justin",
                            IN_MODIFY | IN_CREATE | IN_DELETE );
    length = read( fd, buffer, BUF_LEN );

    if ( length < 0 ) {
        perror( "read" );
    }
    printf("length: %d\n", length);

    while ( i < length ) {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        if ( event->len ) {
            if ( event->mask & IN_CREATE ) {
                if ( event->mask & IN_ISDIR ) {
                    printf( "The directory %s was created.\n", event->name );
                }
                else {
                    printf( "The file %s was created.\n", event->name );
                }
            }
            else if ( event->mask & IN_DELETE ) {
                if ( event->mask & IN_ISDIR ) {
                    printf( "The directory %s was deleted.\n", event->name );
                }
                else {
                    printf( "The file %s was deleted.\n", event->name );
                }
            }
            else if ( event->mask & IN_MODIFY ) {
                if ( event->mask & IN_ISDIR ) {
                    printf( "The directory %s was modified.\n", event->name );
                }
                else {
                    printf( "The file %s was modified.\n", event->name );
                }
            }
        }
        printf("size: %ld\n", event->len);
        i += EVENT_SIZE + event->len;
    }

    ( void ) inotify_rm_watch( fd, wd );
    ( void ) close( fd );

    exit( 0 );
}

