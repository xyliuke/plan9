//
// Created by liuke on 06/03/2017.
//
#include "image_warp.h"
#include <iostream>
namespace plan9
{
    image_warp image_warp::instance() {
        static image_warp imageWarp;
        return imageWarp;
    }

    std::string image_warp::resize(std::string file, int w, int h) {
//        FreeImage_Initialise(TRUE);
//        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(file.c_str());
//        std::cout << "image type" << format ;
        return file;
    }

    image_warp::image_warp() {

    }
}