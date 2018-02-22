//
//  zlib_wrap.hpp
//  anet
//
//  Created by ke liu on 08/11/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#ifndef zlib_wrap_hpp
#define zlib_wrap_hpp

#include <stdio.h>
#include <memory>

namespace plan9 {
    class zlib_wrap {
    public:
        static unsigned long zip(char* data, unsigned long len, char* dst_data, unsigned long dst_len);
        static unsigned long unzip(char* data, unsigned long len, char* dst_data, unsigned long dst_len);
        static char* unzip(char* data, unsigned long len, unsigned long* dst_len);

        static unsigned long gzip(char* data, unsigned long len, char* dst_data, unsigned long dst_len);
        static unsigned long ungzip(char* data, unsigned long len, char* dst_data, unsigned long dst_len);
        static char* ungzip(char* data, unsigned long len, unsigned long* dst_len);
    };
}

#endif /* zlib_wrap_hpp */
