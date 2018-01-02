//
// Created by justin on 27/12/17.
//

#ifndef AUTOSCP_SSHMANAGER_H
#define AUTOSCP_SSHMANAGER_H

#include <libssh/libssh.h>
#include <iostream>
#include <map>

typedef std::map<enum ssh_options_e, const void *> SSH_OPTION_T;

class SSHManager {
public:
    SSHManager(std::string);
    SSHManager(std::string, SSH_OPTION_T options);
    ~SSHManager();

    bool connect();
    bool authenticate(std::string);
    void disconnect();
    void send_ignore();
    ssh_session get_session() const {return session;};

private:
    ssh_session session;
    std::string host;
    int verify_unknownhost();
    //std::string password;
    //std::string directory;
};


#endif //AUTOSCP_SSHMANAGER_H
