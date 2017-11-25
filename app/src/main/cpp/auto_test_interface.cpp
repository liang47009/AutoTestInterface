#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>
#include <netinet/tcp.h>

#include <string>
#include <errno.h>
#include <regex>

#include "auto_test_interface.h"
#include "android-log.h"
#include "ifaddrs.h"

namespace AutoTest {
//void server_on_read(struct bufferevent *bev, void *ctx) {
////    struct timeval start_t;
////    gettimeofday(&start_t, nullptr);
////    struct evbuffer *out = bufferevent_get_output(bev);
//    struct evbuffer *input = bufferevent_get_input(bev);
//    size_t len = evbuffer_get_length(input);
//    char *buf = (char *) malloc(sizeof(char) * len);
//    if (buf == nullptr) {
//        return;
//    }
//    evbuffer_remove(input, buf, len);
//    len = evbuffer_get_length(input);
//    if (ctx) {
//        AutoTestInterface *a = (AutoTestInterface *) ctx;
//        if (a->m_callback) {
//            int fd = bufferevent_getfd(bev);
//            a->m_callback->on_recv(a, buf, fd);
//        } else {
//            LOGI("no set callback server accept client");
//        }
//    }
//    free(buf);
//    buf = nullptr;
//}

    void auto_test_on_readcb(struct bufferevent *bev, void *ctx) {
//    struct timeval start_t;
//    gettimeofday(&start_t, nullptr);
        struct evbuffer *input = bufferevent_get_input(bev);
//        struct evbuffer *out = bufferevent_get_output(bev);
//        evbuffer_add_buffer(out, input);
        size_t len = evbuffer_get_length(input);
        if (len > 0) {
            size_t size = (sizeof(char) * len) + 1;
            char *buf = (char *) malloc(size);
            if (buf == nullptr) {
                return;
            }
            evbuffer_remove(input, buf, len);
            if (ctx) {
                AutoTestInterface *a = (AutoTestInterface *) ctx;
                if (a->m_callback) {
                    a->m_callback->on_recv(a, buf, len);
                } else {
                    LOGI("no set callback server accept client");
                }
            }
            free(buf);
            buf = nullptr;
        }
    }

//void client_event_cb(struct bufferevent *bev, short what, void *ctx) {
//    if (ctx) {
//        AutoTestInterface *a = (AutoTestInterface *) ctx;
//        int fd = bufferevent_getfd(bev);
//        if (a->m_callback) {
//            if (what == BEV_EVENT_CONNECTED) {
//                a->m_callback->on_connect(a, fd);
//            } else if (what == BEV_EVENT_EOF
//                       || what == BEV_EVENT_ERROR
//                       || what == BEV_EVENT_READING
//                       || what == BEV_EVENT_WRITING
//                       || what == BEV_EVENT_TIMEOUT) {
//                a->m_callback->on_error(a, what, fd);
//            }
//        } else {
//            LOGI("no set callback client_event_cb: %d", 0);
//        }
//    }
//    int err = EVUTIL_SOCKET_ERROR();
//    LOGI("socket message, %s", evutil_socket_error_to_string(err));
//}

    void auto_test_event_cb(struct bufferevent *bev, short what, void *ctx) {
        if (ctx) {
            AutoTestInterface *a = (AutoTestInterface *) ctx;
            int fd = bufferevent_getfd(bev);
            if (a->m_callback) {
                if (what & BEV_EVENT_CONNECTED) {
                    int enable = 1;
                    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
                    a->m_callback->on_connect(a, fd);
                } else if (what & BEV_EVENT_EOF
                           || what & BEV_EVENT_ERROR
                           || what & BEV_EVENT_READING
                           || what & BEV_EVENT_WRITING
                           || what & BEV_EVENT_TIMEOUT) {
                    a->m_callback->on_error(a, what, fd);
                }
            } else {
                LOGI("no set callback server_event_cb: %d", 0);
            }
        }
        int err = EVUTIL_SOCKET_ERROR();
        LOGI("socket message, %s", evutil_socket_error_to_string(err));
    }

    void server_evconnlistener_errorcb(struct evconnlistener *, void *ctx) {
        if (ctx) {
            AutoTestInterface *a = (AutoTestInterface *) ctx;
            int err = EVUTIL_SOCKET_ERROR();
            a->m_callback->on_error(a, err, 0);
            LOGI("create server error, %s", evutil_socket_error_to_string(err));
        }
    }

    void auto_test_write_cb(struct bufferevent *bev, void *ctx) {
        if (ctx) {
            AutoTestInterface *a = (AutoTestInterface *) ctx;

        }
    }

    void server_on_accept(struct evconnlistener *listner, evutil_socket_t fd, struct sockaddr *addr,
                          int socklen, void *args) {
        int enable = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
        if (args) {
            AutoTestInterface *a = (AutoTestInterface *) args;
            if (a->m_callback) {
                a->m_callback->on_connect(a, fd);
            } else {
                LOGI("no set callback server accept client: %d", fd);
            }
        }
        struct event_base *base = evconnlistener_get_base(listner);
        struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, auto_test_on_readcb, auto_test_write_cb, auto_test_event_cb, args);
        bufferevent_enable(bev, EV_READ | EV_WRITE);
    }

    void *start_server(void *args) {
        if (args) {
            AutoTestInterface *ati = (AutoTestInterface *) args;
            std::string ip = ati->hi.ip;
            unsigned int port = ati->hi.port;
            struct sockaddr_in remote_addr; //服务器端网络地址结构体
            memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零
            remote_addr.sin_family = AF_INET; //设置为IP通信
            remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());//服务器IP地址
            remote_addr.sin_port = htons(port); //服务器端口号

            struct event_base *server_base = event_base_new();
            struct evconnlistener *listener = evconnlistener_new_bind(server_base, server_on_accept,
                                                                      args,
                                                                      LEV_OPT_CLOSE_ON_FREE |
                                                                      LEV_OPT_REUSEABLE, -1,
                                                                      (struct sockaddr *) &remote_addr,
                                                                      sizeof(remote_addr));
            evconnlistener_set_error_cb(listener, server_evconnlistener_errorcb);
            if (listener) {
                ati->server_base = server_base;
                ati->m_baselistener = listener;
                event_base_dispatch(server_base);
            } else {
                LOGI("create server error!");
            }
        }
        return NULL;
    }

    void *start_client(void *args) {
        if (args) {
            AutoTestInterface *ati = (AutoTestInterface *) args;
            std::string ip = ati->hi.ip;
            unsigned int port = ati->hi.port;
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (-1 == fd) {
                LOGI("create fd error!");
                return NULL;
            }
            struct sockaddr_in remote_addr; //服务器端网络地址结构体
            memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零
            remote_addr.sin_family = AF_INET; //设置为IP通信
            remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());//服务器IP地址
            remote_addr.sin_port = htons(port); //服务器端口号
            struct event_base *client_base = event_base_new();
            struct bufferevent *conn = bufferevent_socket_new(client_base, fd,
                                                              BEV_OPT_CLOSE_ON_FREE);
            if (bufferevent_socket_connect(conn, (struct sockaddr *) &remote_addr,
                                           sizeof(remote_addr)) == 0) {
                bufferevent_setcb(conn, auto_test_on_readcb, auto_test_write_cb, auto_test_event_cb,
                                  args);
                bufferevent_enable(conn, EV_READ | EV_WRITE);
                ati->client_base = client_base;
                ati->conn = conn;
                event_base_dispatch(client_base);
            } else {
                LOGI("============ connect failed! ");
            }
        }
        return NULL;
    }

    void init_client_thread(AutoTestInterface *autoTestInterface) {
        pthread_t thread;
        pthread_create(&thread, NULL, start_client, (void *) autoTestInterface);
        pthread_detach(thread);
    }

    void init_server_thread(AutoTestInterface *autoTestInterface) {
        pthread_t thread;
        pthread_create(&thread, NULL, AutoTest::start_server, (void *) autoTestInterface);
        pthread_detach(thread);
    }

}

AutoTestInterface::AutoTestInterface()
        : m_inited(false), m_mode(0), m_callback(nullptr), conn(nullptr), server_base(nullptr),
          m_baselistener(nullptr), client_base(nullptr) {

}

AutoTestInterface::~AutoTestInterface() {
    if (m_callback) {
        delete m_callback;
        m_callback = nullptr;
    }
    if (conn) {
        bufferevent_free(conn);
        conn = nullptr;
    }
    if (m_baselistener) {
        evconnlistener_free(m_baselistener);
        m_baselistener = nullptr;
    }
    if (server_base) {
        event_base_loopexit(server_base, nullptr);
        event_base_free(server_base);
        server_base = nullptr;
    }
    if (client_base) {
        event_base_loopexit(client_base, nullptr);
        event_base_free(client_base);
        client_base = nullptr;
    }
}

bool AutoTestInterface::start(const char *ip, unsigned int port, int mode) {
    if (!m_inited) {
        this->m_inited = true;
        this->m_mode = mode;
        this->hi.port = port;
        this->hi.ip = ip;
        if (mode == CLIENT_MODE) {
            AutoTest::init_client_thread(this);
        } else if (mode == SERVER_MODE) {
            AutoTest::init_server_thread(this);
        }
    }
    return true;
}

void AutoTestInterface::write_message(const char *msg, size_t len) {
    if (m_callback) {
        m_callback->on_write(this, msg, len);
    }
}

std::string AutoTestInterface::getLocalIPv4() {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        LOGI("get local host ip error");
    }
    int family;
    char host[NI_MAXHOST];
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET) {
            int socklen = sizeof(struct sockaddr_in);
            getnameinfo(ifa->ifa_addr, socklen, host, NI_MAXHOST, nullptr, 0,
                        NI_NUMERICHOST);
            std::string strhost(host);

            int len = strhost.find(".", 0);
            if (len == 3 && (strhost != "127.0.0.1")) {
                return strhost;
            }
        }
    }
    freeifaddrs(ifaddr);
    return std::string();
}
