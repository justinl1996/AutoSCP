//
// Created by justin on 28/12/17.
//

#ifndef AUTOSCP_SCPMANAGER_H
#define AUTOSCP_SCPMANAGER_H

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <time.h>
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
	bool directoryExists(std::string);
    time_t getLastModified(std::string);

private:
    //ssh_scp scp;
    //ssh_session session;

    int mode;
    ssh_ptr_t ssh;
    sftp_session sftp;
	std::chrono::time_point<Clock> time_point;
};


#endif //AUTOSCP_SCPMANAGER_H
