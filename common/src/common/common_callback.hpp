//
//  common_callback.hpp
//  anet
//
//  Created by ke liu on 19/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#ifndef common_callback_hpp
#define common_callback_hpp

#include <string>
#include <memory>

namespace plan9
{
    
class common_callback {
public:
    common_callback(bool success_, int error_code_, std::string reason_) : success(success_), error_code(error_code_), reason(reason_) {
    }

    common_callback() : success(true), error_code(0), reason("success"){
    }

    inline static std::shared_ptr<common_callback> get() {
        return std::make_shared<common_callback>();
    }

    inline static std::shared_ptr<common_callback> get(bool success_, int error_code_, std::string reason_) {
        return std::make_shared<common_callback>(success_, error_code_, reason_);
    }

    bool success;
    int error_code;
    std::string reason;
};
    
}

#endif /* common_callback_hpp */
