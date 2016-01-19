//
// Created by liuke on 15/12/13.
//

#include "log.h"
#include <boost/filesystem.hpp>
#include <fstream>


namespace plan9
{

    log::log() : isInit(false) {

    }

    void log::setFile(std::string filepath) {
        this->filepath = filepath;
        init();
    }

    void log::write(std::string msg) {
        init();
        ofs << msg;
        ofs.flush();
    }

    void log::close() {
        ofs.close();
    }

    log::~log() {
        close();
    }


    void log::init() {
        if (!exist()) {
            ofs.open(filepath, std::ios::app);
            isInit = true;
        }
        if (!isInit) {
            ofs.open(filepath, std::ios::app);
            isInit = true;
        }
    }

    bool log::exist() {
        using namespace boost::filesystem;
        using namespace std;
        path p(filepath);
        path dir = p.parent_path();
        if (!boost::filesystem::exists(filepath)) {
            create_directories(dir);
            return false;
        }
        return true;
    }
}