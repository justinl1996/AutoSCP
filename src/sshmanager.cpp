//
// Created by justin on 27/12/17.
//

#include <libssh/libssh.h>
#include <iostream>
#include <stdexcept>
#include <string.h>

#include "sshmanager.h"


SSHManager::SSHManager(std::string _host) : host(_host)
{
    session = ssh_new();

    if (session == NULL) {
        throw std::runtime_error("Error Initializing SSH session\n");
    }
    const long timeout = 3600;
    ssh_options_set(session, SSH_OPTIONS_HOST, host.c_str());
    ssh_options_set(session, SSH_OPTIONS_TIMEOUT, "3600");
}

SSHManager::SSHManager(std::string _host, SSH_OPTION_T options):
        SSHManager(_host)
{
    for (SSH_OPTION_T::iterator it = options.begin(); it != options.end(); ++it)
    {
        ssh_options_set(session, it->first, it->second);
    }
    //ssh_options_set(session, SSH_OPTIONS_USER, "pi");


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
    verify_unknownhost();
    int rc = ssh_userauth_password(session, NULL, password.c_str());
    if (rc != SSH_AUTH_SUCCESS)
    {
        //printf("%d %d %d %d %d", rc, SSH_AUTH_DENIED, SSH_AUTH_PARTIAL, SSH_AUTH_ERROR, SSH_AUTH_AGAIN);
        fprintf(stderr, "Error authenticating with password: %s\n",
                ssh_get_error(session));
        disconnect();
        return false;
    }
    //verify_unknownhost();
    return true;
}

int SSHManager::verify_unknownhost()
{
    int state, hlen;
    unsigned char *hash = NULL;
    char *hexa;
    char buf[10];
    state = ssh_is_server_known(session);
    hlen = ssh_get_pubkey_hash(session, &hash);
    if (hlen < 0)
        return -1;
    switch (state)
    {
        case SSH_SERVER_KNOWN_OK:
            break; /* ok */
        case SSH_SERVER_KNOWN_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            ssh_print_hexa("Public key hash", hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            free(hash);
            return -1;
        case SSH_SERVER_FOUND_OTHER:
            fprintf(stderr, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            free(hash);
            return -1;
        case SSH_SERVER_FILE_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* fallback to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_SERVER_NOT_KNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
            fprintf(stderr, "Public key hash: %s\n", hexa);
            free(hexa);
            if (fgets(buf, sizeof(buf), stdin) == NULL)
            {
                free(hash);
                return -1;
            }
            if (strncasecmp(buf, "yes", 3) != 0)
            {
                free(hash);
                return -1;
            }
            if (ssh_write_knownhost(session) < 0)
            {
                fprintf(stderr, "Error %s\n", strerror(errno));
                free(hash);
                return -1;
            }
            break;
        case SSH_SERVER_ERROR:
            fprintf(stderr, "Error %s", ssh_get_error(session));
            free(hash);
            return -1;
    }
    free(hash);
    return 0;
}

void SSHManager::send_ignore()
{
    ssh_send_ignore(session, "0");
}

void SSHManager::disconnect()
{
    ssh_disconnect(session);
}

SSHManager::~SSHManager()
{
    ssh_free(session);
}