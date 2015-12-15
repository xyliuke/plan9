//
// Created by liuke on 15/12/13.
//

#include "log.h"
#include <boost/filesystem.hpp>
#include <fstream>


namespace plan9
{
    void log::setFile(std::string filepath) {
        using namespace boost::filesystem;
        using namespace std;
        path p(filepath);
        path dir = p.parent_path();
        if (!boost::filesystem::exists(filepath)) {
            create_directories(dir);
        }
        ofs.open(filepath, ios::app);
    }

    void log::write(std::string msg) {
        ofs << msg;
        ofs.flush();
    }

    void log::close() {
        ofs.close();
    }

    log::~log() {
        close();
    }
}