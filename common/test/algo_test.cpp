//
// Created by liuke on 16/3/27.
//


#include <util/util.h>
#include <util/time.h>
#include <algorithm/crypto_wrap.h>
#include "test/test_def.h"

#ifdef ALGO_TEST

#include "algorithm/compress.h"
#include "algorithm/crypto_wrap.h"

TEST(algo, compress) {
//    int ret_len = 1000;
//    char ret[ret_len];
//
//    int ret_len_1 = 1000;
//    char ret_1[ret_len_1];
//
//
//
//    long long int s = plan9::time::microseconds();
//    std::string src = "hello";
////    std::string src = "{\"args\":{\"server\":0,\"test\":\"hello world from ios\",\"timeout\":5000},\"aux\":{\"action\":\"callback\",\"from\":[\"ID-OTHER-1459089065999386-9950-AL\"],\"id\":\"ID-OTHER-1459089065999386-9950-AL\",\"to\":\"function\"}}";
//    for (int i = 0; i < 1; ++i) {
//        int len = plan9::compress_wrap::compress(src.c_str(), src.length(), ret, &ret_len);
//        std::cout << "compress data len : " << len << std::endl;
//        std::cout << "compress data : " << plan9::util::instance().char_to_string(ret, len) << std::endl;
//        int len_1 = plan9::compress_wrap::decompress(ret, ret_len, ret_1, &ret_len_1);
//    }
//
//    std::cout << "maybe compress size " << plan9::compress_wrap::maybe_compressed_size(1000) << std::endl;
//
//    int t = plan9::time::microseconds() - s;
//    std::cout << "compress time : " << t << std::endl;



}

TEST(algo, compress_file) {
//    char code[7] = "123456";
//    plan9::compress_wrap::compress_zip("./data", code, "./11.zip");
//    plan9::compress_wrap::decompress_zip("./11.zip", code, "/Users/liuke/Desktop/ttt");
//    plan9::compress_wrap::get_file_crc32("/Users/liuke/Desktop/1024.png");
}

TEST(algo, crypt) {
    using namespace plan9;
    std::string s1 = "abc";
    std::cout << s1 << " md5 " << crypto_wrap::MD5(s1) << std::endl;

    std::cout << s1 << " md5 file " << crypto_wrap::MD5_file("./abc.zip") << std::endl;
    std::cout << s1 << " md5 file " << crypto_wrap::MD5_file("./abc1.zip") << std::endl;

    std::cout << s1 << " sha1 " << crypto_wrap::SHA1(s1) << std::endl;
    std::cout << s1 << " sha1 file " << crypto_wrap::SHA1_file("./abc.zip") << std::endl;

    std::cout << s1 << " base64 " << crypto_wrap::base64(s1) << std::endl;
    std::cout << s1 << " base64 decode " << crypto_wrap::base64_decode(crypto_wrap::base64(s1)) << std::endl;

//    crypto_wrap::rsa_generate(1024, "../g_private_key.pem", "../g_public_key.pem", "1234");

//    std::string en = crypto_wrap::rsa_encrypt("g_public_key.pem", "helloworlddsds", "1234");
//    std::string de = crypto_wrap::rsa_decrypt("g_private_key.pem", en);
//    std::cout << "encrypt " << en << std::endl;
//    std::cout << "decrypt " << de << std::endl;

    char p_key[10240], pub_key[10240];
    int p_len = 10240, pub_len = 10240;
    crypto_wrap::create_rsa_private_public_key(p_key, &p_len, pub_key, &pub_len);

//    crypto_wrap::create_rsa_private_public_key_file("./pri.pem", "./pub.pem");
//    std::string en = crypto_wrap::rsa_public_key_file_encrypt_openssl("./rsa_pub.pem", "hello world");
//    std::string de = crypto_wrap::rsa_private_key_file_decrypt_openssl("./rsa_pri2.pem", en);

/*
    de = crypto_wrap::rsa_private_key_file_encrypt_openssl("./pri_key.pem", "heeeee");
    en = crypto_wrap::rsa_public_key_file_decrypt_openssl("./pub_key.pem", de);

    std::string private_key = "-----BEGIN RSA PRIVATE KEY-----\n"
            "MIIEhgIBAAKB/DRhfEImGdWG+Dx6APkAvfmX9pnaOSIbIbK+OA7BJV1oYHrmyoGb\n"
            "CD8km51yfdc7nzC6hazV1Xv8bfhPe6jgaZQ3fZ97REJ1pIn+41VPvS/zMj6k7xcX\n"
            "gOHa/kA98WeNwpvHdY7hJmRMYTVTTClaKiRl6VJoGLdmwHDn+L0fIY2gkGTq1QSB\n"
            "Omn8UNDIVOVAVnj/HzJmEeprWXV6Fcm2eLVdynTR/GussM/+l6hmO0TyGfLHawqP\n"
            "7y+XXKgr7qyYWPwbiHoodXjD74Eu/a+ncRk/pT2vyw0OKaxFiLLQdQ6efe7PpkZ5\n"
            "BcAzEdK0RBcVhHCcnLgdHGysfUsX/wIDAQABAoH8KG5e1UhHhJPLpBKgEZNLEA8G\n"
            "H7dbhkHe/vLonirjxne8kOD3ypZ6BRMisqMfEzVIKPqwF7puMGemEpZ3YuDlXax/\n"
            "YMxq4cmgmYuOudj0tMBJNiVlDzPrHWpF+hxV65J2dnu89+4+DgkOt3R2Qyz/flzT\n"
            "ffIvsdC0FfJYAtK2BdA49mI9HNbc3fPsmzf2KK4HTztGjmuDqTmTchrjgOvM4emo\n"
            "nxAoad7j39Y31ubW2UupI4JOrXcUxL09A6G263yuoz9dg5i60Y68qQpprEACorqE\n"
            "VXUYI7RqLy2V9kHmuxvz9fmIpRHuBfNYQTNRjZkMKwGq0w2EEV3fR1qRAn5/32so\n"
            "oC9qHmOv6C+QP86ebOw6XXV0rDRDW1cHOWGekKm5hC102tjsloMdax+EKjfgrjKv\n"
            "svNRW2cy9LtqqhOPGUhEajqoNp9hyHLPacJ76lFEVKIqTm92X7qieR+xBLhAugVl\n"
            "pZk57sLKI4VfSy6VIJw787dADszvvfQpaKMCfmjdqd+DUavQspwTKmAzQUvVwMva\n"
            "JDOCFiV453BGfC2UkViW6SOrbvN/AcRhC9ZOdz2CBrQG9v2lrUj4faD8kJCwlBVQ\n"
            "uNGEsgQG+97i18GXavBPjWoxuoXoTu9xjg/qdgU2Y0qYsdoxhZxt1AbFEDMMIG3/\n"
            "iUUK3RJNqcl89QJ+JUlaj+AVs1XlL5f+totEjoGoaGR0KW9FVha+S8b5pij8tLHM\n"
            "J3ThKCvDU8K2z+0XmhZxhTYjKSgD8aHXPIUXavSuU/S94er7kqHGiKCIK2XqCIi9\n"
            "OKTib5v+af3ztcRwjb8wb7FX+sKaXiL5cw4TKTz3EaxBVRZ7v7SeC60NAn4xz6jT\n"
            "LLNso5NTo57rVy1a8kvECPhNJyuR3R5CLJjrpbhsljJj6RiovNmACIDw6RCmvM6m\n"
            "UPCdh0cKdE5oW8/nipisj01a6GcOlqqpMqM9p8j6IS+RE9H/rVthTVUnpj0tOKpt\n"
            "Up6YgBFxPpIKLS0GsmntFgaELybXfOB7fskCfhw2LIIePkNZlhW2Y+LHYpZMPAqH\n"
            "kRlrfMHmdqng5wjpbjhPlhZg5OZDxpVe/JzBimVtqRL+iUXUYdyjbAFLOj5Ql9AK\n"
            "GcIY3NGg0/KAasWUOm5cD5zToLJFFKh2fU6xQEbNWhq9ORokzfNspXZh4tj0xnyn\n"
            "apeRkcKH+q7pYQ==\n"
            "-----END RSA PRIVATE KEY-----";
    std::string public_key = "-----BEGIN PUBLIC KEY-----\n"
            "MIIBHDANBgkqhkiG9w0BAQEFAAOCAQkAMIIBBAKB/DRhfEImGdWG+Dx6APkAvfmX\n"
            "9pnaOSIbIbK+OA7BJV1oYHrmyoGbCD8km51yfdc7nzC6hazV1Xv8bfhPe6jgaZQ3\n"
            "fZ97REJ1pIn+41VPvS/zMj6k7xcXgOHa/kA98WeNwpvHdY7hJmRMYTVTTClaKiRl\n"
            "6VJoGLdmwHDn+L0fIY2gkGTq1QSBOmn8UNDIVOVAVnj/HzJmEeprWXV6Fcm2eLVd\n"
            "ynTR/GussM/+l6hmO0TyGfLHawqP7y+XXKgr7qyYWPwbiHoodXjD74Eu/a+ncRk/\n"
            "pT2vyw0OKaxFiLLQdQ6efe7PpkZ5BcAzEdK0RBcVhHCcnLgdHGysfUsX/wIDAQAB\n"
            "-----END PUBLIC KEY-----";

    en = crypto_wrap::rsa_public_key_encrypt_openssl(public_key, "12345");
    de = crypto_wrap::rsa_private_key_decrypt_openssl(private_key, en);

    en = crypto_wrap::rsa_private_key_encrypt_openssl(private_key, "67890");
    de = crypto_wrap::rsa_public_key_decrypt_openssl(public_key, en);
    */
}

#endif