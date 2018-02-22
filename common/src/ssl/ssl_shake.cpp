//
// Created by ke liu on 16/12/2017.
// Copyright (c) 2017 ke liu. All rights reserved.
//

#include "ssl_shake.h"
#include <common/tri_bool.h>
#include <openssl/ssl.h>
#include <assert.h>
#include <iostream>
#include <openssl/err.h>
#include <openssl/x509_vfy.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <map>
#include <vector>
#include <cert/cert.h>


namespace plan9
{
#define WHERE_INFO(ssl, w, flag, msg) { \
    if(w & flag) { \
      printf("\t"); \
      printf(msg); \
      printf(" - %s ", SSL_state_string(ssl)); \
      printf(" - %s ", SSL_state_string_long(ssl)); \
      printf("\n"); \
    }\
 }

    static void dummy_ssl_info_callback(const SSL* ssl, int where, int ret) {
        if(ret == 0) {
            printf("dummy_ssl_info_callback, error occured.\n");
            return;
        }
        WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
        WHERE_INFO(ssl, where, SSL_CB_EXIT, "EXIT");
        WHERE_INFO(ssl, where, SSL_CB_READ, "READ");
        WHERE_INFO(ssl, where, SSL_CB_WRITE, "WRITE");
        WHERE_INFO(ssl, where, SSL_CB_ALERT, "ALERT");
        WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
    }

    static void dummy_ssl_msg_callback(int writep ,int version ,int contentType ,const void* buf
            ,size_t len ,SSL* ssl ,void *arg ) {
        printf("\tMessage callback with length: %zu   write %d version %d contentType %d \n", len, writep, version, contentType);
//        std::cout << "\tMessage callback " << std::string((char*)buf, len);
    }

    class ssl_shake::ssl_shake_impl {
    public:

        static std::map<SSL*, ssl_shake_impl*> ssl_impl_map;
        static std::shared_ptr<std::vector<X509*>> ca;

        static bool verify_cert(X509* cert) {
            if (ca) {
                static X509_STORE* store = NULL;
                if (store == NULL) {
                    store = X509_STORE_new();
                    auto it = ca->begin();
                    while (it != ca->end()) {
                        X509* x = *it;
                        X509_STORE_add_cert(store, x);
                        it ++;
                    }
                }

                X509_STORE_CTX* store_ctx = X509_STORE_CTX_new();
                X509_STORE_CTX_init(store_ctx, store, cert, 0);
//                X509_STORE_CTX_set_depth(store_ctx, 3);
                int ret = X509_verify_cert(store_ctx);
                return ret == 1;
            }
            return true;
        }

        static int verify_callback(int ok, X509_STORE_CTX* ctx) {
//            X509* cert = X509_STORE_CTX_get_current_cert(ctx);
//            BIO* bio = BIO_new(BIO_s_mem());
//            X509_print(bio, cert);
//            char buf[10240];
//            int ret = BIO_read(bio, buf, 10240);
//            printf("%s", buf);



            SSL* ssl = (SSL*)X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
            if (ssl_impl_map.find(ssl) != ssl_impl_map.end()) {
                ssl_shake_impl* impl = ssl_impl_map[ssl];
                bool validate_domain = impl->validate_domain_bool;
                bool validate_cert = impl->validate_cert_bool;

                if (validate_domain) {
                    int  err = X509_STORE_CTX_get_error(ctx);
                    if (err == X509_V_ERR_HOSTNAME_MISMATCH) {
                        impl->set_invalidation_domain_result(true);
                        return 0;
                    }
                }

                if (validate_cert) {
                    if (impl->has_validated_cert == tri_true) {
                        return 1;
                    } else if (impl->has_validated_cert == tri_false) {
                        return 0;
                    }
                    stack_st_X509* chain = X509_STORE_CTX_get1_chain(ctx);
                    int num = sk_X509_num(chain);
                    if (num > 0) {
                        X509* v = sk_X509_value(chain, num - 1);
                        bool ver = verify_cert(v);
                        impl->has_validated_cert = tri_true;
                        impl->set_invalidation_cert_result(!ver);
                        if (ver) {
                            return 1;
                        } else {
                            return 0;
                        }
                    }
                }
            }

            return 1;
        }

        ssl_shake_impl() : buf((char*)malloc(buf_len)), ctx(nullptr), validate_cert_bool(false), validate_domain_bool(false),
                        has_validated_cert(tri_undefined), invalidate_domain_result(false), invalidate_cert_result(false) {
            ssl = SSL_new(get_ssl_ctx());
            read_bio = BIO_new(BIO_s_mem());
            write_bio = BIO_new(BIO_s_mem());
            SSL_set_bio(ssl, read_bio, write_bio);
            SSL_set_verify_depth(ssl, 2);
            ssl_impl_map[ssl] = this;
            if (!ca) {
                cert::get_ca_cert([=](std::shared_ptr<std::vector<X509*>> list) mutable {
                    ca = list;
                });
            }
        }

        ~ssl_shake_impl() {
            if (ssl_impl_map.find(ssl) != ssl_impl_map.end()) {
                ssl_impl_map.erase(ssl);
            }
            if (buf != nullptr) {
                delete buf;
                buf = nullptr;
            }
            if (ssl != nullptr) {
                SSL_free(ssl);
                ssl = nullptr;
            }
            if (ctx != nullptr) {
                SSL_CTX_free(ctx);
                ctx = nullptr;
            }
        }
        void set_host(std::string host) {
            SSL_set_tlsext_host_name(ssl, host.c_str());
            X509_VERIFY_PARAM* param = SSL_get0_param(ssl);
            X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
            X509_VERIFY_PARAM_set1_host(param, host.c_str(), host.length());
        }

        void write(char *data, long len, std::function<void(std::shared_ptr<common_callback>, char *data, long len)> callback) {
            if (callback) {
                if (ssl && write_bio) {
                    int ret = SSL_write(ssl, data, (int)len);
                    if (ret > 0) {
                        int bytes_read = 0;
                        while((bytes_read = BIO_read(write_bio, buf, buf_len)) > 0) {
                            callback(common_callback::get(), buf, bytes_read);
                        }
                        return;
                    }
                }
                callback(common_callback::get(false, -1, "ssl write error"), nullptr, -1);
            }
        }

        void on_connect(int tcp_id, std::function<void(std::shared_ptr<common_callback>)> callback) {
            SSL_set_connect_state(ssl);     // 这是个客户端连接
            SSL_do_handshake(ssl);
            bool finish = do_shake_finish(tcp_id);
            if (finish) {
                if (callback) {
                    callback(common_callback::get());
                }
            }
        }

        void on_read(int tcp_id, char* data, long len, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<char>, long)> callback) {
            if (SSL_is_init_finished(ssl)) {
                if (callback) {
                    int ret = BIO_write(read_bio, data, (int)len);
                    if (ret >= 0) {
                        static int num = 10240;
                        std::shared_ptr<char> buf(new char[num]{});
                        ret = SSL_read(ssl, buf.get(), num);
                        std::shared_ptr<common_callback> ccb = std::make_shared<common_callback>();
                        if (ret < 0) {
                            ccb->success = false;
                            ccb->error_code = -1;
                            ccb->reason = "ssl read error";
                        } else if (ret == 0){
                            ccb->success = false;
                            ccb->error_code = -2;
                            ccb->reason = "ssl close";
                        }
                        callback(ccb, buf, ret);
                    }
                }
            } else {
                int written = BIO_write(read_bio, data, (int)len);
                if (written > 0 && do_shake_finish(tcp_id)) {
                    if (callback) {
                        callback(common_callback::get(), nullptr, -1);
                    }
                }
            }
        }

        void validate_domain(bool validate) {
            validate_domain_bool = validate;
        }

        void validate_cert(bool validate) {
            validate_cert_bool = validate;
        }

        bool is_domain_invalidation() {
            return invalidate_domain_result;
        }
        bool is_cert_invalidation() {
            return invalidate_cert_result;
        }

        void set_invalidation_domain_result(bool invalidation) {
            invalidate_domain_result = invalidation;
        }

        void set_invalidation_cert_result(bool invalidation) {
            invalidate_cert_result = invalidation;
        }

    private:
        bool do_shake_finish(int tcp_id) {
            if (!SSL_is_init_finished(ssl)) {
                int ret = SSL_connect(ssl);     // 开始握手。这个
                write(tcp_id);
                if (ret != 1) {
                    int err = SSL_get_error(ssl, ret);
                    if (err == SSL_ERROR_WANT_READ) {
                        write(tcp_id);
                    } else if (err == SSL_ERROR_WANT_WRITE) {
                    }
                } else {
                    return true;
                }
                return false;
            }
            return true;
        }

        void write(int tcp_id) {
            int bytes_read = 0;
            while((bytes_read = BIO_read(write_bio, buf, buf_len)) > 0) {
                uv_wrapper::write_uv(tcp_id, buf, bytes_read, nullptr);
            }
        };

        SSL_CTX* get_ssl_ctx() {
            if (!ctx) {
                SSL_library_init();
                OpenSSL_add_all_algorithms();
                SSL_load_error_strings();
                ERR_load_BIO_strings();
                ctx = SSL_CTX_new(SSLv23_client_method());
//                SSL_CTX_set_info_callback(ctx, dummy_ssl_info_callback);
//                SSL_CTX_set_msg_callback(ctx, dummy_ssl_msg_callback);
                SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
                assert(ctx);
            }
            return ctx;
        }
    private:
        SSL_CTX* ctx;
        SSL* ssl;
        BIO* read_bio;
        BIO* write_bio;
        char* buf;
        static int buf_len;
        bool validate_domain_bool;//是否验证域名
        bool validate_cert_bool;//是否验证证书
        tri_bool has_validated_cert;//已经验证通过了的标志，避免多次验证
        bool invalidate_domain_result;
        bool invalidate_cert_result;
    };

    int ssl_shake::ssl_shake_impl::buf_len = 10240;
    std::map<SSL*, ssl_shake::ssl_shake_impl*> ssl_shake::ssl_shake_impl::ssl_impl_map;
    std::shared_ptr<std::vector<X509*>> ssl_shake::ssl_shake_impl::ca;

    ssl_shake::ssl_shake( ) : impl(new ssl_shake_impl) {

    }

    void ssl_shake::set_host(std::string host) {
        impl->set_host(host);
    }

    void ssl_shake::write(char *data, long len, std::function<void(std::shared_ptr<common_callback>, char *data, long len)> callback) {
        impl->write(data, len, callback);
    }

    void ssl_shake::on_connect(int tcp_id, std::function<void(std::shared_ptr<common_callback>)> callback) {
        impl->on_connect(tcp_id, callback);
    }

    void ssl_shake::on_read(int tcp_id, char* data, long len, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<char>, long)> callback) {
        impl->on_read(tcp_id, data, len, callback);
    }

    void ssl_shake::validate_domain(bool validate) {
        impl->validate_domain(validate);
    }

    void ssl_shake::validate_cert(bool validate) {
        impl->validate_cert(validate);
    }

    bool ssl_shake::is_domain_invalidation() {
        return impl->is_domain_invalidation();
    }

    bool ssl_shake::is_cert_invalidation() {
        return impl->is_cert_invalidation();
    }
}
