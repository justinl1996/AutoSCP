//
// Created by justin on 30/12/17.
//

#include "filemanager.h"
#include "filewatcher.h"
#ifndef _WIN32
#include "filewatcherlinux.h"
#endif

FileManager::FileManager(std::string _source, std::string _dest, SCPManager &_scp):
        source(_source), dest(_dest), scp(_scp)
{
    //FileWatcher *fw = new FileWatcherLinux(source);
#ifndef _WIN32
    filewatch = std::unique_ptr<FileWatcher>(new FileWatcherLinux(source));
#else
    //todo: add window's implementation here
#endif
}

void FileManager::start()
{
    while(true) {
        filewatch->watch();

        std::cout << "NewFile: \n";
        for (auto file: filewatch->getNewfile()) {
            std::cout << file << std::endl;
            filewatch->getRootPath(file);
        }

        std::cout << "Modified: \n";
        for (auto file: filewatch->getModified()) {
            std::cout << file << std::endl;
        }
        std::cout << "Deleted: \n";
        for (auto file: filewatch->getDeleted()) {
            std::cout << file << std::endl;
        }
    }
}

std::string FileManager::getParentPath(std::string path)
{
    boost::filesystem::path p(path);
    return p.parent_path().string();
}

std::string FileManager::getEndPath(std::string path)
{
    boost::filesystem::path p(path);
    return p.stem().string();
}

std::string FileManager::getRelativePath(std::string path, std::string root)
{
    return path.substr(root.size() + 1, path.length());
}

std::string FileManager::joinPath(std::string lhs, std::string rhs)
{
    return (boost::filesystem::path(lhs) /
                             boost::filesystem::path(rhs)).make_preferred().string();
}


void FileManager::syncAll()
{
    std::string root = getParentPath(source);

    auto directory_f = [=](std::string dir) {
        std::string path = getRelativePath(dir, root);
        scp.createDirectory(joinPath(dest, path));
        //std::cout << path << std::endl;
    };

    auto file_f = [=](std::string file) {
        //std::cout << "file: " << getRelativePath(file, root) << std::endl;

        scp.copyFile(file, joinPath(dest, getRelativePath(file, root)));
    };


    //scp.copyFile("/home/justin/fp.cfg", "test/fp.cfg");//"/home/justin/fp.cfg.bak");
    filewatch->traverse_directory(source, file_f, directory_f);

    //scp.create_directory("yellow");
}