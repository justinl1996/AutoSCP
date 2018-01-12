
#include <stdlib.h>
#include <string>

#include "fileutils.h"
#include "filewatcherwindows.h"

std::string wcharToString(LPCWSTR p, size_t len)
{
	//char *temp = new char[len + 1];
	std::vector<char> temp(len + 1);

	WideCharToMultiByte(CP_UTF8, 0, p, len-1, &temp[0], len-1, NULL, NULL);
	temp[len] = '\0';

	std::string ret(&temp[0], len - 1);
	//delete temp;
	return ret;
}

FileWatcherWindows::FileWatcherWindows(std::string directory) :
	FileWatcher(directory)
{
    fileMon = ::CreateFile( directory.c_str(),
                                 FILE_LIST_DIRECTORY,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 NULL, // security descriptor
                                 OPEN_EXISTING, // how to create
                                 FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, // file attributes
                                 NULL ); // file with attributes to copy
}

void FileWatcherWindows::begin_read()
{
	memset(&overlap, 0, sizeof(overlap));
	BOOL success = ::ReadDirectoryChangesW(fileMon,
		read_buffer,
		sizeof(read_buffer),
		TRUE, // recursive?
		FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
		0x0,
		&overlap,
		0x0);
}

void FileWatcherWindows::watch()
{
	std::string original_name;

	while (1)
	{
		begin_read();
		DWORD bytes;
		BOOL res = ::GetOverlappedResult(fileMon,
			&overlap,
			&bytes,
			true //blocking? 
		);

		if (res != TRUE)
			return;

		FILE_NOTIFY_INFORMATION* ev = (FILE_NOTIFY_INFORMATION*)read_buffer;

		do
		{
			std::string temp = wcharToString(ev->FileName, ev->FileNameLength / 2 + 1);
			temp = FileUtils::joinPath(full_path, temp);
			switch (ev->Action)
			{
			case FILE_ACTION_ADDED:
			{
				//todo: investigate why adding a file also triggers a 2x modified event
				/*if (PathIsDirectory(temp.c_str())) {
					printf("DIR HERE\n");
				}*/
				DWORD attr = GetFileAttributes(temp.c_str());
				if (attr == FILE_ATTRIBUTE_DIRECTORY) {
					new_directories.push_back(temp);
				}
				else {
					new_files.push_back(temp);
				}
				//printf("%x\n", attr);
				printf("added filename: %s\n", temp.c_str());
			}

			break;
			case FILE_ACTION_REMOVED:
			{
				printf("removed filename: %s\n", temp.c_str());
				deleted.push_back(temp);
			}
			break;
			case FILE_ACTION_MODIFIED:
			{
				printf("modified filename: %s\n", temp.c_str());
				DWORD attr = GetFileAttributes(temp.c_str());
				if (attr != FILE_ATTRIBUTE_DIRECTORY) {
					modified.push_back(temp);
				}
			}
			break;
			case FILE_ACTION_RENAMED_OLD_NAME:
			{
				original_name = temp;
				printf("renamed old filename: %s\n", temp.c_str());

			}
			break;
			case FILE_ACTION_RENAMED_NEW_NAME:
			{
				renamed.emplace_back(original_name, temp);
				printf("renamed new filename: %s\n", temp.c_str());
			}
			break;
			default:
				printf("unhandled action %d\n", ev->Action);
			}

			if (ev->NextEntryOffset == 0)
				break;
			ev = (FILE_NOTIFY_INFORMATION*)((char*)ev + ev->NextEntryOffset);
		} while (true);
	}
}

void FileWatcherWindows::stop()
{

}