//
//  string_parser.hpp
//  anet
//
//  Created by ke liu on 22/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#ifndef string_parser_hpp
#define string_parser_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <memory>


namespace plan9 {
    class string_parser {
    public:
        static std::string trim(std::string string);
        static std::string to_lower(std::string string);
        static std::string to_upper(std::string string);
        static std::shared_ptr<std::vector<std::string>> split(std::string string, std::string split_string);
        static int dex_to_dec(char* dex, long len);
        static int dex_to_dec(char c);
    };
}

#endif /* string_parser_hpp */
