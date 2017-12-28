//
// Created by justin on 28/12/17.
//

#include <libssh/libssh.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "scpmanager.h"


SCPManager::SCPManager(ssh_session _session, int _mode): session(_session), mode(_mode)
{
    scp = ssh_scp_new(session, mode | SSH_SCP_RECURSIVE, ".");
    if (scp == NULL)
    {
        std::runtime_error("Error allocating scp session: " + std::string(ssh_get_error(session)));
    }
}

bool SCPManager::connect()
{


}