//
// Created by justin on 28/12/17.
//

#ifndef AUTOSCP_SCPMANAGER_H
#define AUTOSCP_SCPMANAGER_H

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <iostream>
#include <memory>
#include <chrono>
#include "sshmanager.h"
#if __linux__
#include <sys/stat.h>
#endif


class SCPManager {

public:
    typedef std::unique_ptr<SSHManager> ssh_ptr_t;
	typedef std::chrono::high_resolution_clock Clock;

    SCPManager(ssh_ptr_t, int);
    ~SCPManager();
    //bool connect();
    //void disconnect();
    void ping();
    int createDirectory(std::string);
    int renameFile(std::string, std::string);
    int copyFile(std::string, std::string);
    int deleteFile(std::string);

private:
    //ssh_scp scp;
    //ssh_session session;
    ssh_ptr_t ssh;
    sftp_session sftp;
    int mode;
	std::chrono::time_point<Clock> time_point;

	mode_t getFilePermissions(std::string);
    mode_t getDirectoryPermissions(std::string);
};


#endif //AUTOSCP_SCPMANAGER_H
