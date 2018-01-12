//
// Created by justin on 30/12/17.
//

#include <boost/thread.hpp>
#include "filemanager.h"
#include "filewatcher.h"
#include "fileutils.h"
#ifdef _WIN32
#include "filewatcherwindows.h"
#else
#include "filewatcherlinux.h"
#endif

FileManager::FileManager(std::string _source, std::string _dest, SCPManager::ssh_ptr_t _ssh):
        source(_source), dest(_dest)
{
    //FileWatcher *fw = new FileWatcherLinux(source);
    scp = std::unique_ptr<SCPManager>(new SCPManager(std::move(_ssh), SSH_SCP_WRITE));
#ifdef _WIN32
	filewatch = std::unique_ptr<FileWatcher>(new FileWatcherWindows(source));
#else
    filewatch = std::unique_ptr<FileWatcher>(new FileWatcherLinux(source));
    //todo: add window's implementation here
#endif
}

void FileManager::start()
{
    std::string root = FileUtils::getParentPath(source);
    auto thread = boost::thread(std::bind(&FileWatcher::watch, filewatch.get()));
    //todo: use semphores instead
    while(true) {
        //std::cout << "NewFile: \n";
        //for (auto file: filewatch->getNewfile()) {
		//filewatch->watch();

        std::string file;
        while ((file = filewatch->getNewfile()) != "") {
            //std::cout << file << std::endl;
            //todo: avoid nesting these functions
			//printf("ASSADS\n");
			scp->copyFile(file, FileUtils::toUnixPath(
				FileUtils::joinPath(dest, FileUtils::getRelativePath(file, root))));
        }

		while ((file = filewatch->getNewDirectories()) != "") {
			//std::cout << file << std::endl;
			scp->createDirectory(FileUtils::toUnixPath(
				FileUtils::joinPath(dest, FileUtils::getRelativePath(file, root))));
		}
            //}

        //std::cout << "Modified: \n";
        while ((file = filewatch->getModified()) != "") {
            //std::cout << file << std::endl;
            scp->copyFile(file, FileUtils::toUnixPath(
				FileUtils::joinPath(dest, FileUtils::getRelativePath(file, root))));
        }
        //std::cout << "Deleted: \n";
        while ((file = filewatch->getDeleted()) != "") {

            //std::cout << file << std::endl;
            scp->deleteFile(FileUtils::toUnixPath(FileUtils::getRelativePath(file, root)));

            //fprintf(stderr, "%s\n", sftp_get_error(scp.get));
        }
        r_pair_t renamed = filewatch->getRenamed();
        //std::cout << renamed.first << std::endl;
        while(renamed.first != "") {
            //std::cout << renamed.first << "->" << renamed.second << std::endl;
            std::string from = FileUtils::toUnixPath(FileUtils::getRelativePath(renamed.first, root));
            std::string to = FileUtils::toUnixPath(FileUtils::getRelativePath(renamed.second, root));

            std::cout << scp->renameFile(from, to) << std::endl;
            renamed = filewatch->getRenamed();
        }


        filewatch->clearAll();
        //todo: ping after 1min inactivity + find windows version
        //usleep(500);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
        scp->ping();
    }
}

void FileManager::syncAll()
{
    std::string root = FileUtils::getParentPath(source);
	

    auto directory_f = [=](std::string dir) {
        std::string relative_path = FileUtils::toUnixPath(FileUtils::getRelativePath(dir, root));

        //std::cout << relative_path << std::endl;
		scp->createDirectory(FileUtils::joinPathLinux(dest, relative_path));
    };

    auto file_f = [=](std::string file) {
        //std::cout << "file: " << getRelativePath(file, root) << std::endl;
		//server we are pushing to should be unix based
		std::string relative_path = FileUtils::toUnixPath(FileUtils::getRelativePath(file, root));
		//std::cout << relative_path << std::endl;

        if (scp->getLastModified(relative_path) < FileUtils::getLastModified(file)) {
		//scp->getLastModified(relative_path);
			std::cout << relative_path << std::endl;

            scp->copyFile(file, relative_path);
        }
        //scp->copyFile(file, relative_path);
        //scp->getFileAttributes(relative_path);

    };


    //scp.copyFile("/home/justin/fp.cfg", "test/fp.cfg");//"/home/justin/fp.cfg.bak");
    filewatch->traverse_directory(source, file_f, directory_f);
    //scp.deleteFile("test/fp.cfg");
    //scp.create_directory("yellow");
}