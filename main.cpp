#include <iostream>
#include <fstream>
#include <stdio.h>
#include <librsync.h>
#include <stdlib.h>
#include <libssh/libssh.h>
#include <string.h>

#include "sshmanager.h"
#include "scpmanager.h"
#include "filemanager.h"
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

    std::ofstream outfile ("signature",std::ofstream::binary);
    outfile.write(rs_out, length - rs_buffer->avail_out);
    outfile.close();

    rs_job_free(job);
    delete buffer;
    delete rs_buffer;
    delete rs_out;

}

void testSignature2 ()
{
    rs_stats_t stats;
    FILE *fin = fopen("nothing", "rb");
    FILE *fsig = fopen("signature", "wb");
    rs_result res = rs_sig_file(fin, fsig, RS_DEFAULT_BLOCK_LEN,
            RS_MAX_STRONG_SUM_LENGTH, RS_BLAKE2_SIG_MAGIC, &stats);
    if (res == RS_DONE) {
        printf("SUCCESS\n");
    }
    fclose(fin);
    fclose(fsig);
}


void testDelta()
{
    rs_stats_t stats;
    rs_signature_t *sig;

    FILE *fp = fopen("signature", "rb");
    rs_result result = rs_loadsig_file(fp, &sig, &stats);
    fclose(fp);

    if (result == RS_DONE ) {
        printf("DONE!\n");
    }

    //rs_job_t *job = rs_loadsig_begin(&sig);


    rs_job_t *job = rs_delta_begin(sig);
    std::ifstream is ("nothing2", std::ifstream::binary);
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
    rs_buffers_t *rs_buffer = new rs_buffers_t;
    rs_buffer->next_in = buffer;
    rs_buffer->avail_in = length;
    rs_buffer->eof_in = 0;

    rs_buffer->next_out = rs_out;
    rs_buffer->avail_out = length;


    rs_job_iter(job, rs_buffer);

    std::ofstream outfile ("delta",std::ofstream::binary);
    outfile.write(rs_out, length - rs_buffer->avail_out);
    outfile.close();


    rs_job_free(job);
    //delete sig;
    //delete job;
}

void testDelta2()
{
    rs_stats_t stats;
    rs_signature_t *sig;

    FILE *fp = fopen("signature", "rb");
    rs_result result = rs_loadsig_file(fp, &sig, &stats);
    fclose(fp);

    if (result == RS_DONE ) {
        printf("SUCCESS\n");
    }

    rs_build_hash_table(sig);

    FILE *fin = fopen("nothing2", "rb");
    FILE *fdelta = fopen("delta", "wb");

    rs_result res = rs_delta_file(sig, fin, fdelta, &stats);

    if (res == RS_DONE ) {
        printf("SUCCESS\n");
    }

    fclose(fin);
    fclose(fdelta);
}

void testPatch()
{
    FILE *fin = fopen("nothing", "rb");
    FILE *fdelta = fopen("delta", "rb");
    FILE *fout = fopen("patched", "wb");
    rs_stats_t stats;

    rs_result res = rs_patch_file(fin, fdelta, fout, &stats);
    if (res == RS_DONE ) {
        printf("SUCCESS\n");
    }

    fclose(fin);
    fclose(fdelta);
    fclose(fout);
}

int show_remote_processes(ssh_session session)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;
    channel = ssh_channel_new(session);
    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, "ls -l");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {
        if (write(1, buffer, nbytes) != (unsigned int) nbytes)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    if (nbytes < 0)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return SSH_ERROR;
    }
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

#define PI_HOST "192.168.200.2"
#define PI_USER "pi"

#define MOSS_HOST "moss.labs.eait.uq.edu.au"
#define MOSS_USER "s4371057"


int main(int argc, char **argv )
{
#if 0
    FileWatcherLinux filewatch("/home/justin/test");


    //filewatch.stop();
#endif

#if 1
    SSH_OPTION_T options;
    options[SSH_OPTIONS_USER] = MOSS_USER;
    SSHManager ssh(MOSS_HOST, options);
    //SSHManager ssh( , options);


    bool ret;
    ret = ssh.connect();
    if (!ret) {
        //std::cout << "Error Connecting\n";
        exit(1);
    }

    char *password = getpass("password: ");
    ret = ssh.authenticate(password);
    if (!ret) {
        //std::cout << "Error Authenticating\n";
        //ssh.authenticate("maple");
        exit(1);
    }
    std::cout << "SUCCESS\n";
    ssh_session session = ssh.get_session();

    SCPManager *scp = new SCPManager(session, SSH_SCP_WRITE);
    //scp.create_directory(session, "SADSDA/asashd/qwey");
#endif

    FileManager manager("/home/justin/test", "test2", *scp);
    manager.syncAll();

    //printf("END\n");
    return 0;
}


