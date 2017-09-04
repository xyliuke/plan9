//
// Created by liuke on 23/11/2016.
//
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <sstream>
#include <files.h>
#include "hex.h"
#include <boost/filesystem.hpp>
//#include <cryptopp565/validate.h>
//#include <cryptopp565/randpool.h>
#include "sha.h"
#include "base64.h"
#include "crypto_wrap.h"
#include "md5.h"
#include "rsa.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>


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

    std::string rsa_public_op_data(std::shared_ptr<RSA> rsa, std::string text, bool encrypt, bool* error) {
        if (rsa == nullptr) {
            *error = true;
            return "";
        }
        unsigned long flen = text.length();
        int rsa_len = RSA_size(rsa.get());
        unsigned char* p_en = (unsigned char *)malloc(rsa_len + 1);
        memset(p_en, 0 ,rsa_len + 1);
        int result = -1;
        if (encrypt) {
            result = RSA_public_encrypt((int)flen, (unsigned char *)text.c_str(), p_en, rsa.get(), RSA_PKCS1_PADDING);
        } else {
            result = RSA_public_decrypt((int)flen, (unsigned char *)text.c_str(), p_en, rsa.get(), RSA_PKCS1_PADDING);
        }
        if (result < 0){
            *error = true;
            return "";
        }
        std::string ret((char*)p_en, result);
        free(p_en);
        *error = false;
        return ret;
    }

    std::string rsa_private_op_data(std::shared_ptr<RSA> rsa, std::string text, bool encrypt, bool* error) {
        if (rsa == nullptr) {
            *error = true;
            return "";
        }
        int flen = text.length();
        int rsa_len=RSA_size(rsa.get());
        char* p_en = (char *)malloc(rsa_len + 1);
        memset(p_en, 0 ,rsa_len + 1);
        int result = -1;
        if (encrypt) {
            result = RSA_private_encrypt(flen, (unsigned char *)text.c_str(), (unsigned char*)p_en, rsa.get(), RSA_PKCS1_PADDING);
        } else {
            result = RSA_private_decrypt(flen, (unsigned char *)text.c_str(), (unsigned char*)p_en, rsa.get(), RSA_PKCS1_PADDING);
        }
        if (result < 0) {
            *error = true;
            return "";
        }
        std::string ret(p_en, result);
        free(p_en);
        *error = false;
        return ret;
    }


    std::shared_ptr<RSA> get_rsa_from_file(std::string key_file, bool pub_key) {
        FILE* file;
        if((file = fopen(key_file.c_str(), "r")) == NULL){
            return nullptr;
        }
        std::shared_ptr<RSA> ret;
        if (pub_key) {
            ret.reset(PEM_read_RSA_PUBKEY(file,NULL,NULL,NULL));
            if (ret == nullptr) {
                fclose(file);
                return nullptr;
            }
        } else {
            ret.reset(PEM_read_RSAPrivateKey(file,NULL,NULL,NULL));
            if (ret == nullptr) {
                fclose(file);
                return nullptr;
            }
        }
        fclose(file);
        return ret;
    }

    std::shared_ptr<RSA> get_rsa_from_content(std::string key_content, bool pub_key) {
        std::shared_ptr<BIO> bio(BIO_new_mem_buf((void*)(key_content.c_str()), key_content.length()));
        std::shared_ptr<RSA> ret;
        if (pub_key) {
            ret.reset(PEM_read_bio_RSA_PUBKEY(bio.get(), NULL, NULL, NULL));
        } else {
            ret.reset(PEM_read_bio_RSAPrivateKey(bio.get(), NULL, NULL, NULL)) ;
        }
        return ret;
    }


    std::string crypto_wrap::rsa_public_key_file_encrypt(std::string key, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_file(key, true);
        std::string ret = rsa_public_op_data(rsa, text, true, error);
        return ret;
    }

    std::string crypto_wrap::rsa_private_key_file_decrypt(std::string key, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_file(key, false);
        std::string ret = rsa_private_op_data(rsa, text, false, error);
        return ret;
    }

    std::string crypto_wrap::rsa_private_key_file_encrypt(std::string key, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_file(key, false);
        std::string ret = rsa_private_op_data(rsa, text, true, error);
        return ret;
    }

    std::string crypto_wrap::rsa_public_key_file_decrypt(std::string key, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_file(key, true);
        std::string ret = rsa_public_op_data(rsa, text, false, error);
        return ret;
    }

    std::string crypto_wrap::rsa_public_key_encrypt(std::string key_content, std::string text, bool *error) {
        auto rsa = get_rsa_from_content(key_content, true);
        std::string ret = rsa_public_op_data(rsa, text, true, error);
        return ret;
    }

    std::string crypto_wrap::rsa_public_key_decrypt(std::string key_content, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_content(key_content, true);
        std::string ret = rsa_public_op_data(rsa, text, false, error);
        return ret;
    }

    std::string crypto_wrap::rsa_private_key_encrypt(std::string key_content, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_content(key_content, false);
        std::string ret = rsa_private_op_data(rsa, text, true, error);
        return ret;
    }

    std::string crypto_wrap::rsa_private_key_decrypt(std::string key_content, std::string text, bool *error) {
        std::shared_ptr<RSA> rsa = get_rsa_from_content(key_content, false);
        std::string ret = rsa_private_op_data(rsa, text, false, error);
        return ret;
    }

    bool crypto_wrap::create_rsa_private_public_key_file(std::string private_key_file, std::string public_key_file, int length) {
        BIGNUM *bne = BN_new();
        int ret = BN_set_word(bne, RSA_3);
        if(ret != 1){
            BN_free(bne);
            return false;
        }

        RSA* rsa = RSA_new();
        ret = RSA_generate_key_ex(rsa, length, bne, NULL);
        if (ret != 1) {
            RSA_free(rsa);
            BN_free(bne);
            return false;
        }

        BIO* bp_public(BIO_new_file(public_key_file.c_str(), "w+"));
        ret = PEM_write_bio_RSA_PUBKEY(bp_public, rsa);
        if(ret != 1){
            BIO_free_all(bp_public);
            RSA_free(rsa);
            BN_free(bne);
            return false;
        }
        BIO_free_all(bp_public);

        BIO *bp_private = BIO_new_file(private_key_file.c_str(), "w+");
        ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL);
        BIO_free_all(bp_private);

        RSA_free(rsa);
        BN_free(bne);

        return ret == 1;
    }

    bool crypto_wrap::create_rsa_private_public_key(char *private_key, int *private_key_len, char *public_key,
                                                    int *public_key_len, int length) {
        BIGNUM *bne = BN_new();
        int ret = BN_set_word(bne, RSA_3);
        if(ret != 1){
            BN_free(bne);
            return false;
        }

        RSA* rsa = RSA_new();
        ret = RSA_generate_key_ex(rsa, length, bne, NULL);
        if (ret != 1) {
            RSA_free(rsa);
            BN_free(bne);
            return false;
        }

        BIO* bio_pub = BIO_new(BIO_s_mem());
        ret = PEM_write_bio_RSA_PUBKEY(bio_pub, rsa);
        if(ret == 1){
            ret = BIO_read(bio_pub, public_key, *public_key_len);
            *public_key_len = ret;
            BIO_free_all(bio_pub);
        } else {
            BIO_free_all(bio_pub);
            RSA_free(rsa);
            BN_free(bne);
            return false;
        }

        BIO* bio_pri = BIO_new(BIO_s_mem());
        ret = PEM_write_bio_RSAPrivateKey(bio_pri, rsa, NULL, NULL, 0, NULL, NULL);
        if(ret == 1){
            ret = BIO_read(bio_pri, private_key, *private_key_len);
            *private_key_len = ret;
            BIO_free_all(bio_pri);
        } else {
            BIO_free_all(bio_pri);
            RSA_free(rsa);
            BN_free(bne);
            return false;
        }

        RSA_free(rsa);
        BN_free(bne);

        return true;
    }

}
