//
// Created by liuke on 06/03/2017.
//



#include <test/test_def.h>
#include "image/image_warp.h"
#ifdef IMAGE_TEST

TEST(image, resize) {
    plan9::image_warp::instance().resize("./1.jpg", 1, 1);
}

#endif
