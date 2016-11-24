//
// Created by liuke on 23/11/2016.
//
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <sstream>
#include <files.h>
#include <hex.h>
#include <boost/filesystem.hpp>
#include <sha.h>
#include <base64.h>
#include "crypto_wrap.h"
#include "md5.h"


namespace plan9
{

    static const char alp[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    std::string crypto_wrap::MD5(std::string text) {
        CryptoPP::Weak::MD5 md5;
        CryptoPP::SecByteBlock digest(md5.DigestSize());
        md5.Update((byte*)text.c_str(), text.length());
        md5.Final(digest);
        std::stringstream ss;
        for (int i = 0; i < digest.m_size; ++i) {
            byte b = digest[i];
            int b1 = (b & 0xF0) >> 4;
            int b2 = (b & 0x0F);
            ss << alp[b1];
            ss << alp[b2];
        }
        return ss.str();
    }

    std::string crypto_wrap::MD5_file(std::string file) {
        using namespace boost::filesystem;
        if (exists(file) && is_regular_file(file)) {
            CryptoPP::Weak::MD5 md5;
            const size_t size = (size_t)(CryptoPP::Weak1::MD5::DIGESTSIZE * 2);
            byte buf[size] = {0};
            CryptoPP::FileSource(file.c_str(), true,
                                 new CryptoPP::HashFilter(md5, new CryptoPP::HexEncoder( new CryptoPP::ArraySink(buf, size))));

            std::string strHash = std::string(reinterpret_cast<const char*>(buf), size);
            return strHash;
        } else {
            return "";
        }
    }

    std::string crypto_wrap::SHA1(std::string text) {
        CryptoPP::SHA1 sha1;
        CryptoPP::SecByteBlock digest(sha1.DigestSize());
        sha1.Update((byte*)text.c_str(), text.length());
        sha1.Final(digest);
        std::stringstream ss;
        for (int i = 0; i < digest.m_size; ++i) {
            byte b = digest[i];
            int b1 = (b & 0xF0) >> 4;
            int b2 = (b & 0x0F);
            ss << alp[b1];
            ss << alp[b2];
        }
        return ss.str();
    }

    std::string crypto_wrap::SHA1_file(std::string file) {
        using namespace boost::filesystem;
        if (exists(file) && is_regular_file(file)) {
            CryptoPP::SHA1 sha1;
            const size_t size = (size_t) CryptoPP::SHA1::DIGESTSIZE * 2;
            byte buf[size] = {0};
            CryptoPP::FileSource(file.c_str(), true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(
                    new CryptoPP::HexEncoder(new CryptoPP::ArraySink(buf, size)))));
            std::string strHash = std::string(reinterpret_cast<const char *>(buf), size);
            return strHash;
        } else {
            return "";
        }
    }

    std::string crypto_wrap::base64(std::string text) {
        std::string ret;
        CryptoPP::StringSource(text, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(ret)));
        if (ret.at(ret.length() - 1) == '\n') {
            return ret.substr(0, ret.length() - 1);
        }
        return ret;
    }
    std::string crypto_wrap::base64_decode(std::string text) {
        std::string ret;
        CryptoPP::StringSource(text, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(ret)));
        return ret;
    }

}
