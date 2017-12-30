//
// Created by justin on 28/12/17.
//

#include <libssh/libssh.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>
#include <fcntl.h>
#include <bits/ios_base.h>
#include "scpmanager.h"


SCPManager::SCPManager(ssh_session session, int _mode): mode(_mode)
{
    sftp = sftp_new(session);
    if (sftp == NULL)
    {
        std::runtime_error("Error allocating SFTP session: " +
                                   std::string(ssh_get_error(session)));
    }

    int rc = sftp_init(sftp);
    if (rc != SSH_OK)
    {
        std::runtime_error("Error initializing SFTP session: " +
                                   std::string(ssh_get_error(sftp)));
        sftp_free(sftp);
    }
}

int SCPManager::createDirectory(std::string directory)
{

    int rc;
    rc = sftp_mkdir(sftp, directory.c_str(), S_IRWXU);
    if (rc != SSH_OK)
    {
        if (sftp_get_error(sftp) != SSH_FX_FILE_ALREADY_EXISTS)
        {
            fprintf(stderr, "Can't create directory\n");
                    //ssh_get_error(session));
            return rc;
        }
    }


    /*int access_type = O_WRONLY | O_CREAT | O_TRUNC;
    sftp_file file;
    const char *helloworld = "Hello, World!\n";
    int length = strlen(helloworld);
    int rc, nwritten;

    file = sftp_open(sftp, "helloworld/helloworld.txt",
                     access_type, S_IRWXU);
    if (file == NULL)
    {
        fprintf(stderr, "Can't open file for writing: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }
    nwritten = sftp_write(file, helloworld, length);
    if (nwritten != length)
    {
        fprintf(stderr, "Can't write data to file: %s\n",
                ssh_get_error(session));
        sftp_close(file);
        return SSH_ERROR;
    }
    rc = sftp_close(file);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't close the written file: %s\n",
                ssh_get_error(session));
        return rc;
    }
    return SSH_OK;*/
}

int SCPManager::copyFile(std::string source, std::string dest_path)
{
    int access = O_WRONLY | O_CREAT;
    sftp_file file;
    std::ifstream infile(source, std::ifstream::binary);


    infile.seekg (0, infile.end);
    int length = infile.tellg();
    infile.seekg (0, infile.beg);

    char *buffer = new char[length];
    infile.read(buffer, length);
    infile.close();

#if 1
    std::ofstream outfile(dest_path, std::ifstream::binary);
    outfile.write(buffer, length);
    outfile.close();
#endif
    delete [] buffer;
}

mode_t SCPManager::getPermissions(std::string file)
{
#if _WIN32
    return S_IRWXU;
#else
    struct stat buf;
    stat(file.c_str(), &buf);
    return buf.st_mode;
#endif
}

SCPManager::~SCPManager()
{
    sftp_free(sftp);
}