//
// Created by liuke on 06/03/2017.
//

#ifndef COMMON_IMAGE_WARP_H
#define COMMON_IMAGE_WARP_H

#include <string>

namespace plan9
{
    class image_warp {

    public:
        static image_warp instance();

        std::string resize(std::string file, int w, int h);
    private:
        image_warp();
    };

}



#endif //COMMON_IMAGE_WARP_H
