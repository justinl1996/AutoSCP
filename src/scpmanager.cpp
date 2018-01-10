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
#include <vector>
//#include <bits/ios_base.h>
#include "scpmanager.h"


SCPManager::SCPManager(ssh_ptr_t _ssh, int _mode): mode(_mode), ssh(std::move(_ssh))
{
    ssh_session session = ssh->get_session();
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
	time_point = Clock::now();
}

int SCPManager::createDirectory(std::string directory)
{
    int rc;
	mode_t permission = getDirectoryPermissions(directory);
	rc = sftp_mkdir(sftp, directory.c_str(), permission);//S_IRWXU);
    if (rc != SSH_OK)
    {
        if (sftp_get_error(sftp) != SSH_FX_FILE_ALREADY_EXISTS)
        {
            fprintf(stderr, "Can't create directory %s\n", directory.c_str(),
                    ssh_get_error(ssh->get_session()));
                    //ssh_get_error(session));
            return rc;
        }
    }
	time_point = Clock::now();
    return SSH_OK;

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

int SCPManager::renameFile(std::string old_p, std::string new_p)
{
	time_point = Clock::now();
    return sftp_rename(sftp, old_p.c_str(), new_p.c_str());
}

int SCPManager::deleteFile(std::string file)
{
    int rc = sftp_unlink(sftp, file.c_str());
    if (rc) {
        fprintf(stderr, "Error deleting file: %s\n", file.c_str());
    }
	time_point = Clock::now();
    return SSH_OK;
}

int SCPManager::copyFile(std::string source, std::string dest_path)
{
    int access = O_WRONLY | O_CREAT;
    sftp_file file;
    std::ifstream infile(source, std::ifstream::binary);
    //fprintf(stderr, "fail: %d\n", infile.fail());
    if (infile.fail()) {
        fprintf(stderr, "Cannot open file for copying: %s\n", source.c_str());
        return SSH_ERROR;
    }

    infile.seekg (0, infile.end);
    int length = infile.tellg();
    infile.seekg (0, infile.beg);

    //printf("length %d\n", length);
    char *buffer = new char[length];
    infile.read(buffer, length);
    infile.close();

#if 0
    std::ofstream outfile(dest_path, std::ifstream::binary);
    outfile.write(buffer, length);
    outfile.close();
#endif

    mode_t permissions = getFilePermissions(source);
    file = sftp_open(sftp, dest_path.c_str(), access, permissions);
    if (file == NULL) {
        fprintf(stderr, "Cannot open file %s for writing. Error: %s\n", dest_path.c_str(),
                ssh_get_error(ssh->get_session()));

        return SSH_ERROR;
    }

    int nwritten = sftp_write(file, buffer, length);
    if (nwritten != length) {
        fprintf(stderr, "Cannot write data to file %s", source.c_str());
        return SSH_ERROR;
    }
    sftp_close(file);
	time_point = Clock::now();

    delete [] buffer;
    return SSH_OK;
}

void SCPManager::ping()
{
	using namespace std::chrono;
	//only need to send every 30 seconds
	if (duration_cast<seconds>(Clock::now() - time_point).count() > 30) {
		//printf("HERE\n");
		time_point = Clock::now();
		ssh->send_ignore();
	}
}

mode_t SCPManager::getFilePermissions(std::string file)
{
#if _WIN32
	return 0x180; //500
#else
	struct stat buf;
	stat(file.c_str(), &buf);
	return buf.st_mode;
#endif
}

mode_t SCPManager::getDirectoryPermissions(std::string file)
{
#if _WIN32
	return 0x1c0; //700
#else
	//return getFilePermissions(file);
    return 0x1c0;
#endif
}

SCPManager::~SCPManager()
{
    sftp_free(sftp);
}