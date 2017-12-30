//
// Created by justin on 28/12/17.
//

#ifndef AUTOSCP_SCPMANAGER_H
#define AUTOSCP_SCPMANAGER_H

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <iostream>
#if __linux__
#include <sys/stat.h>
#endif
class SCPManager {


public:

    SCPManager(ssh_session, int);
    ~SCPManager();
    //bool connect();
    //void disconnect();
    int createDirectory(std::string);
    int copyFile(std::string, std::string);
private:
    //ssh_scp scp;
    //ssh_session session;
    sftp_session sftp;
    int mode;

    mode_t getPermissions(std::string);
};


#endif //AUTOSCP_SCPMANAGER_H
