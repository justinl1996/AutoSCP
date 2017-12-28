//
// Created by justin on 28/12/17.
//

#ifndef AUTOSCP_SCPMANAGER_H
#define AUTOSCP_SCPMANAGER_H

#include <libssh/libssh.h>

class SCPManager {


public:

    SCPManager(ssh_session session, mode);
    ~SCPManager();
    bool connect();
    void disconnect;


private:
    ssh_scp scp;
    ssh_session session;
    int mode;
};


#endif //AUTOSCP_SCPMANAGER_H
