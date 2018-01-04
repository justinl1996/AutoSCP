#include <iostream>
#include <fstream>
#include <stdio.h>
//#include <librsync.h>
#include <stdlib.h>
#include <libssh/libssh.h>
#include <string.h>

#include "sshmanager.h"
#include "scpmanager.h"
#include "filemanager.h"
#include "filewatcher.h"
#include "filewatcherlinux.h"


#define PI_HOST "192.168.200.2"
#define PI_USER "pi"

#define MOSS_HOST "moss.labs.eait.uq.edu.au"
#define MOSS_USER "s4371057"

#ifdef _WIN32
#include <Windows.h>


std::string getpass(const char *prompt, bool show_asterisk = true)
{
	const char BACKSPACE = 8;
	const char RETURN = 13;

	std::string password;
	unsigned char ch = 0;

	std::cout << prompt << std::endl;

	DWORD con_mode;
	DWORD dwRead;

	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(hIn, &con_mode);
	SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

	while (ReadConsoleA(hIn, &ch, 1, &dwRead, NULL) && ch != RETURN)
	{
		if (ch == BACKSPACE)
		{
			if (password.length() != 0)
			{
				if (show_asterisk)
					std::cout << "\b \b";
				password.resize(password.length() - 1);
			}
		}
		else
		{
			password += ch;
			if (show_asterisk)
				std::cout << '*';
		}
	}
	std::cout << std::endl;
	return password;
}

#endif

class A {
public:
    int i;
    A() {
        i = 0;
    }

    void foo() {
        //printf("IN THREAD");
        std::cout << "IN THREAD\n" << std::endl;
        i += 5;
        return ;
    }

};

void hello_world()
{
    printf("ASDSAD");
}

#include <boost/thread.hpp>


int main(int argc, char **argv )
{
#if 0
    FileWatcherLinux filewatch("/home/justin/test");


    //filewatch.stop();
#endif

#if 1
    SSH_OPTION_T options;
    options[SSH_OPTIONS_USER] = MOSS_USER;
    auto ssh = std::unique_ptr<SSHManager>(new SSHManager(MOSS_HOST, options));
    //SSHManager ssh( , options);


    bool ret;
    ret = ssh->connect();
    if (!ret) {
        //std::cout << "Error Connecting\n";
        exit(1);
    }

	std::string password = getpass("password: ");


	ret = ssh->authenticate(password);
    if (!ret) {
        //std::cout << "Error Authenticating\n";
        //ssh.authenticate("maple");
        exit(1);
    }
    std::cout << "SUCCESS\n";
    //ssh_session session = ssh.get_session();
    //SCPManager *scp = new SCPManager(std::move(ssh), SSH_SCP_WRITE);
    //scp.create_directory(session, "SADSDA/asashd/qwey");
#endif
#if 1
    FileManager manager("C:\\Users\\justin\\Documents\\nothing", "", std::move(ssh));
    manager.syncAll();
    manager.start();
#endif
#if 0
    A a;
    boost::thread *th = new boost::thread(boost::bind(&A::foo, &a));

    //boost::thread *th = new boost::thread(hello_world);

    //printf("END\n");
    //th->join();
    th->join();
    printf("i: %d\n", a.i);
    //while(1) {} ;
#endif
    return 0;
}


