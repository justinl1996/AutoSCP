#include <iostream>
#include <fstream>
#include <stdio.h>
#include <librsync.h>


#include "filewatcher.h"
#include "filewatcherlinux.h"
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



using namespace std;
typedef std::vector<std::string>::iterator  StrIter;

void testFileWatcher()
{
    FileWatcherLinux filewatch("/home/justin");
    filewatch.watch();


    std::vector<std::string> modified = filewatch.getModified();
    std::vector<std::string> deleted = filewatch.getDeleted();

    for (StrIter p = modified.begin(); p != modified.end(); p++)
    {
        printf("modified file: %s\n", (*p).c_str());
    }

    for (StrIter p = deleted.begin(); p != deleted.end(); p++)
    {
        printf("deleted file: %s\n", (*p).c_str());
    }

    //std::cout << "changed file\n";
    filewatch.stop();
}

void testSignature()
{
    std::ifstream is ("nothing", std::ifstream::binary);
    char *buffer;
    int length = 0;

    if (is) {
        // get length of file:
        is.seekg(0, is.end);
        length = is.tellg();
        is.seekg(0, is.beg);

        // allocate memory:
        buffer = new char[length];

        // read data as a block:
        is.read(buffer, length);

        is.close();
    }

    char *rs_out = new char[length];
    rs_job_t *job = rs_sig_begin (RS_DEFAULT_BLOCK_LEN,
                                  RS_MAX_STRONG_SUM_LENGTH, RS_BLAKE2_SIG_MAGIC);

    rs_buffers_t *rs_buffer = new rs_buffers_t;

    rs_buffer->next_in = buffer;
    rs_buffer->avail_in = length;
    rs_buffer->eof_in = 0;

    rs_buffer->next_out = rs_out;
    rs_buffer->avail_out = length;

    rs_job_iter(job, rs_buffer);

    std::ofstream outfile ("new",std::ofstream::binary);
    outfile.write(rs_out, length - rs_buffer->avail_out);


    outfile.close();


    delete buffer;
    delete rs_buffer;
    delete rs_out;
}




int main(int argc, char **argv )
{
    

    printf("%s", rs_librsync_version);
}

