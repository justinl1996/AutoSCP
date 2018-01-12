#ifndef AUTOSCP_FILEWATCHERWINDOWS_H
#define AUTOSCP_FILEWATCHERWINDOWS_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "filewatcher.h"


class FileWatcherWindows : public FileWatcher
{
public:
	FileWatcherWindows(std::string _directory);

	void watch();
	void stop();

private:
	//typedef std::map<int, std::string> dir_map_t;

	void begin_read();
	//int fd;
	//dir_map_t wd_path;
	//std::vector<std::string> get_all_directories(std::string);
	HANDLE fileMon;
	OVERLAPPED overlap;
	DWORD read_buffer[2048];

};


static std::string wcharToString(LPCWSTR, size_t);

#endif
