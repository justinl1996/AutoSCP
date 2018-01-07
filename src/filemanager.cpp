//
// Created by justin on 30/12/17.
//

#include <boost/thread.hpp>
#include "filemanager.h"
#include "filewatcher.h"
#include "fileutils.h"
#ifndef _WIN32
#include "filewatcherlinux.h"
#endif

FileManager::FileManager(std::string _source, std::string _dest, SCPManager::ssh_ptr_t _ssh):
        source(_source), dest(_dest)
{
    //FileWatcher *fw = new FileWatcherLinux(source);
    scp = std::unique_ptr<SCPManager>(new SCPManager(std::move(_ssh), SSH_SCP_WRITE));
#ifndef _WIN32
    filewatch = std::unique_ptr<FileWatcher>(new FileWatcherLinux(source));
#else
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
            std::cout << file << std::endl;
            scp->copyFile(file, FileUtils::joinPath(dest, FileUtils::getRelativePath(file, root)));
        }
            //}

        //std::cout << "Modified: \n";
        while ((file = filewatch->getModified()) != "") {
            std::cout << file << std::endl;
            scp->copyFile(file, FileUtils::joinPath(dest, FileUtils::getRelativePath(file, root)));
        }
        //std::cout << "Deleted: \n";
        while ((file = filewatch->getDeleted()) != "") {

            std::cout << file << std::endl;
            scp->deleteFile(FileUtils::getRelativePath(file, root));

            //fprintf(stderr, "%s\n", sftp_get_error(scp.get));
        }
        r_pair_t renamed = filewatch->getRenamed();
        //std::cout << renamed.first << std::endl;
        while(renamed.first != "") {
            std::cout << renamed.first << "->" << renamed.second << std::endl;
            std::string from = FileUtils::getRelativePath(renamed.first, root);
            std::string to = FileUtils::getRelativePath(renamed.second, root);

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


        std::cout << relative_path << std::endl;
		scp->createDirectory(FileUtils::joinPathLinux(dest, relative_path));
        //std::cout << path << std::endl;
    };

    auto file_f = [=](std::string file) {
        //std::cout << "file: " << getRelativePath(file, root) << std::endl;
		//server we are pushing to should be unix based
		std::cout << FileUtils::getRelativePath(file, root) << std::endl;
		//std::string full_path = FileUtils::joinPathLinux(dest, relative_path);
		std::string full_path = FileUtils::toUnixPath(FileUtils::getRelativePath(file, root));
		scp->copyFile(file, full_path);


    };


    //scp.copyFile("/home/justin/fp.cfg", "test/fp.cfg");//"/home/justin/fp.cfg.bak");
    filewatch->traverse_directory(source, file_f, directory_f);
    //scp.deleteFile("test/fp.cfg");
    //scp.create_directory("yellow");
}