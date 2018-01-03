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

    //auto thread = boost::thread(std::bind(&FileWatcher::watch, filewatch.get()));



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
        filewatch->clearAll();

        //usleep(500);
        scp->ping();
    }
}

void FileManager::syncAll()
{
    std::string root = FileUtils::getParentPath(source);

    auto directory_f = [=](std::string dir) {
        std::string path = FileUtils::getRelativePath(dir, root);
        scp->createDirectory(FileUtils::joinPath(dest, path));
        //std::cout << path << std::endl;
    };

    auto file_f = [=](std::string file) {
        //std::cout << "file: " << getRelativePath(file, root) << std::endl;

        scp->copyFile(file, FileUtils::joinPath(dest, FileUtils::getRelativePath(file, root)));
    };


    //scp.copyFile("/home/justin/fp.cfg", "test/fp.cfg");//"/home/justin/fp.cfg.bak");
    filewatch->traverse_directory(source, file_f, directory_f);
    //scp.deleteFile("test/fp.cfg");
    //scp.create_directory("yellow");
}