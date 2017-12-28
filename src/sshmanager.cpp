//
// Created by justin on 27/12/17.
//

#include <libssh/libssh.h>
#include <iostream>
#include <stdexcept>

#include "sshmanager.h"


SSHManager::SSHManager(std::string _host) : host(_host)
{
    session = ssh_new();

    if (session == NULL) {
        throw std::runtime_error("Error Initializing SSH session\n");
    }
    ssh_options_set(session, SSH_OPTIONS_HOST, host.c_str());
}

SSHManager::SSHManager(std::string _host, SSH_OPTION_T options):
        SSHManager(_host)
{
    for (SSH_OPTION_T::iterator it = options.begin(); it != options.end(); ++it)
    {
        ssh_options_set(session, it->first, it->second);
    }

}

bool SSHManager::connect()
{
    int rc = ssh_connect(session);

    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to localhost: %s\n",
                ssh_get_error(session));
        //ssh_free(session);
        return false;
    }
    return true;
}

bool SSHManager::authenticate(std::string password)
{
    int rc = ssh_userauth_password(session, NULL, password.c_str());
    if (rc != SSH_AUTH_SUCCESS)
    {
        fprintf(stderr, "Error authenticating with password: %s\n",
                ssh_get_error(session));
        disconnect();
        return false;
    }

    return true;
}

void SSHManager::disconnect()
{
    ssh_disconnect(session);
}

SSHManager::~SSHManager()
{
    ssh_free(session);
}