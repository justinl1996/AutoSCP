#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <libssh/libssh.h>
#include <string.h>
#include <boost/date_time.hpp>

#include "fileutils.h"
#include "sshmanager.h"
#include "scpmanager.h"
#include "filemanager.h"
#include "filewatcher.h"
#include "filewatcherlinux.h"
#include "inih/INIReader.h"


#define PI_HOST "192.168.200.2"
#define PI_USER "pi"

#define MOSS_HOST "moss.labs.eait.uq.edu.au"
#define MOSS_USER "s4371057"


#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include "filewatcherwindows.h"


struct Settings {
	std::string user;
	std::string host;
	std::string localdir;
	std::string remotedir;
};

std::unique_ptr<Settings> init(std::string ini)
{
	INIReader reader(ini);
	if (reader.ParseError() < 0) {
		std::cout << "Can't load " << ini << std::endl;
		return NULL;
	}
	
	const std::string def_str = "UNKNOWN";
	auto s = std::make_unique<Settings>();
	
	s->user = reader.Get("General", "user", def_str);
	s->host = reader.Get("General", "host", def_str);
	s->localdir = reader.Get("General", "localdir", def_str);
	s->remotedir = reader.Get("General", "remotedir", def_str);
	
	if (s->user == def_str || s->localdir == def_str || s->host == def_str
		|| s->remotedir == def_str) {
		std::cout << "corrupted " << ini << std::endl;
		return NULL;
	}

	using namespace boost::filesystem;
	if (!exists(path(s->localdir))) {
		std::cout << "No such directory exists: " << s->localdir << std::endl;
		return NULL;
	}

	return s;
}


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

int main(int argc, char **argv )
{
#if 0
    FileWatcherLinux filewatch("/home/justin/test");


    //filewatch.stop();
#endif

#if 1
	auto settings = init("AutoSCP.ini");
	if (settings == NULL) {
		exit(1);
	};
	//std::cout << "host: " << settings->host << std::endl;
	//std::cout << "user: " << settings->user << std::endl;

    SSH_OPTION_T options;
    options[SSH_OPTIONS_USER] = (settings->user).c_str();
    auto ssh = std::unique_ptr<SSHManager>(new SSHManager(settings->host, options));
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
        exit(1);
    }
    std::cout << "SUCCESS\n";
	std::cout << "SYNCING...\n";
    FileManager manager(settings->localdir, settings->remotedir, std::move(ssh));
	if (!manager.checkRemoteDir()) {
		std::cout << "Remote Directory " << settings->remotedir << " does not exists\n";
		exit(1);
	}
    manager.syncAll();
	std::cout << "STARTING...." << std::endl;
    manager.start();
#endif
#if 1
	//boost::posix_time::ptime timeLocal =
	//	boost::posix_time::second_clock::local_time();
	//std::cout << "Current System Time = " << timeLocal << std::endl;
	
		/*
	INIReader reader("AutoSCP.ini");
	if (reader.ParseError() < 0) {
		std::cout << "Can't load 'test.ini'\n";
		exit(1);
	}

	std::cout << "user: " << reader.Get("General", "remotedir", "UNKNOWN") << std::endl;
	time_t time = FileUtils::getLastModified("C:\\Users\\justin\\Documents\\nothing\\gh.txt");
	struct tm * timeinfo;
	timeinfo = localtime(&time);
	printf("date/time is: %s\n", asctime(timeinfo));
	*/
#endif
#if 0
	(void)argc; (void)argv;
	fswatcher_t watcher = fswatcher_create(FSWATCHER_CREATE_DEFAULT, FSWATCHER_EVENT_ALL, "c:\\users\\justin\\Documents\\nothing", 0x0);

	while (true)
	{
		fswatcher_event_handler handler = { watch_event_handler };
		fswatcher_poll(watcher, &handler, 0x0);
	};

	fswatcher_destroy(watcher);
#endif
#if 0
	auto filewatch = std::unique_ptr<FileWatcher>(
		new FileWatcherWindows("c:\\users\\justin\\Documents\\nothing"));
	while (1) {
		filewatch->watch();
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
	}
#endif
    return 0;
}


