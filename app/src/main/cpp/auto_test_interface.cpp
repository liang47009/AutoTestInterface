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

#include "auto_test_interface.h"
#include "android-log.h"

void server_on_read(struct bufferevent *bev, void *ctx) {
//    struct timeval start_t;
//    gettimeofday(&start_t, nullptr);
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *out = bufferevent_get_output(bev);
    size_t len = evbuffer_get_length(input);
    char *buf = (char *) malloc(sizeof(char) * len);
    if (buf == nullptr) {
        return;
    }
    evbuffer_remove(input, buf, len);
    len = evbuffer_get_length(input);
    if (ctx) {
        AutoTestInterface *a = (AutoTestInterface *) ctx;
        if (a->m_callback) {
            int fd = bufferevent_getfd(bev);
            a->m_callback->on_recv(a, buf, fd);
        } else {
            LOGI("no set callback server accept client");
        }
    }
    free(buf);
    buf = nullptr;
}

void client_on_readcb(struct bufferevent *bev, void *ctx) {
//    struct timeval start_t;
//    gettimeofday(&start_t, nullptr);
    struct evbuffer *input = bufferevent_get_input(bev);
    size_t len = evbuffer_get_length(input);

    char *buf = (char *) malloc(sizeof(char) * len);
    if (buf == nullptr) {
        return;
    }
    evbuffer_remove(input, buf, len);
    len = evbuffer_get_length(input);
    if (ctx) {
        AutoTestInterface *a = (AutoTestInterface *) ctx;
        if (a->m_callback) {
            int fd = bufferevent_getfd(bev);
            a->m_callback->on_recv(a, buf, fd);
        } else {
            LOGI("no set callback server accept client");
        }
    }
    free(buf);
    buf = nullptr;
}

void client_event_cb(struct bufferevent *bev, short what, void *ctx) {
    if (ctx) {
        AutoTestInterface *a = (AutoTestInterface *) ctx;
        int fd = bufferevent_getfd(bev);
        if (a->m_callback) {
            if (what == BEV_EVENT_CONNECTED) {
                a->m_callback->on_connect(a, fd);
            } else if (what == BEV_EVENT_EOF
                       || what == BEV_EVENT_ERROR
                       || what == BEV_EVENT_READING
                       || what == BEV_EVENT_WRITING
                       || what == BEV_EVENT_TIMEOUT) {
                a->m_callback->on_error(a, what, fd);
            }
        } else {
            LOGI("no set callback client_event_cb: %d", 0);
        }
    }
    int err = EVUTIL_SOCKET_ERROR();
    LOGI("socket error, %s", evutil_socket_error_to_string(err));
}

void server_event_cb(struct bufferevent *bev, short what, void *ctx) {
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
                int fd = bufferevent_getfd(bev);
                a->m_callback->on_error(a, what, fd);
            }
        } else {
            LOGI("no set callback server_event_cb: %d", 0);
        }
    }
    LOGI("server_event_cb, %d", what);
}

void server_evconnlistener_errorcb(struct evconnlistener *, void *ctx) {
    if (ctx) {
        AutoTestInterface *a = (AutoTestInterface *) ctx;
        int err = EVUTIL_SOCKET_ERROR();
        a->m_callback->on_error(a, err, 0);
        LOGI("create server error, %s", evutil_socket_error_to_string(err));
    }
}

void write_cb(struct bufferevent *bev, void *ctx) {
    if (ctx) {
        AutoTestInterface *a = (AutoTestInterface *) ctx;

    }
}

void server_on_accept(struct evconnlistener *listner, evutil_socket_t fd, struct sockaddr *addr,
                      int socklen, void *args) {
    LOGI("accept a client: fd:%d , socklen:%d, addr:%s", fd, socklen, addr->sa_data);
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
    bufferevent_setcb(bev, server_on_read, write_cb, server_event_cb, args);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void *start_server(void *args) {
    if (args) {
        AutoTestInterface *ati = (AutoTestInterface *) args;
        const char *ip = ati->hi.ip;
        unsigned int port = ati->hi.port;
        struct sockaddr_in remote_addr; //服务器端网络地址结构体
        memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零
        remote_addr.sin_family = AF_INET; //设置为IP通信
        remote_addr.sin_addr.s_addr = inet_addr(ip);//服务器IP地址
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
        const char *ip = ati->hi.ip;
        unsigned int port = ati->hi.port;
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == fd) {
            LOGI("create fd error!");
            return NULL;
        }
        struct sockaddr_in remote_addr; //服务器端网络地址结构体
        memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零
        remote_addr.sin_family = AF_INET; //设置为IP通信
        remote_addr.sin_addr.s_addr = inet_addr(ip);//服务器IP地址
        remote_addr.sin_port = htons(port); //服务器端口号
        struct event_base *client_base = event_base_new();
        struct bufferevent *conn = bufferevent_socket_new(client_base, fd, BEV_OPT_CLOSE_ON_FREE);
        if (bufferevent_socket_connect(conn, (struct sockaddr *) &remote_addr,
                                       sizeof(remote_addr)) == 0) {
            bufferevent_setcb(conn, client_on_readcb, write_cb, client_event_cb, args);
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

int issetugid() {
//    LOGI("============ issetugid ");
    return 0;
}

void arc4random_addrandom(unsigned char *c, int i) {
//    LOGI("============ arc4random_addrandom: %s, %d", c, i);
}

void init_client_thread(AutoTestInterface *autoTestInterface) {
    pthread_t thread;
    pthread_create(&thread, NULL, start_client, (void *) autoTestInterface);
    pthread_detach(thread);
}

/**
 * 创建一个新线程，在新线程里初始化libevent读事件的相关设置，并开启event_base_dispatch
 */
void init_server_thread(AutoTestInterface *autoTestInterface) {
    pthread_t thread;
    pthread_create(&thread, NULL, start_server, (void *) autoTestInterface);
    pthread_detach(thread);
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
            init_client_thread(this);
        } else {
            init_server_thread(this);
        }
    }
    return true;
}

void AutoTestInterface::write_message(std::string msg) {
    if (m_callback) {
        m_callback->on_write(this, msg);
    }
}
