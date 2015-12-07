//
// Created by liuke on 15/10/27.
//

#include <glog/logging.h>
#include "log.h"



namespace plan9 {

    void log::init(std::string log_dir) {
        google::InitGoogleLogging("common");
        google::FlushLogFiles(google::INFO);
        google::InstallFailureSignalHandler();
        google::SetLogFilenameExtension(".log");
        google::SetLogDestination(google::INFO, (log_dir + "/info_").c_str());
        google::SetLogDestination(google::WARNING, (log_dir + "/warn_").c_str());
        google::SetLogDestination(google::ERROR, (log_dir + "error_").c_str());
        google::SetLogDestination(google::FATAL, (log_dir + "fatal_").c_str());
        FLAGS_stderrthreshold = google::INFO;
        FLAGS_alsologtostderr = true;
        FLAGS_log_dir = log_dir;
        FLAGS_minloglevel = 0;
        FLAGS_max_log_size = 1;
        FLAGS_colorlogtostderr = true;
    }

    void log::logI(std::string msg) {
        LOG(INFO) << msg;
    }


    void log::logW(std::string msg) {
        LOG(WARNING) << msg;
    }

    void log::logE(std::string msg) {
        LOG(ERROR) << msg;
    }

    void log::logF(std::string msg) {
        LOG(FATAL) << msg;
    }

}