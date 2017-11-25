#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/http.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>
#include <event2/listener.h>
#include <netinet/tcp.h>

#include <string>

#include "auto_test_interface.h"
#include "android-log.h"

void server_on_read(struct bufferevent *bev, void *ctx) {
    struct timeval start_t;
    gettimeofday(&start_t, nullptr);
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
            a->m_callback->on_read(a, buf, len);
        } else {
            LOGI("no set callback server accept client");
        }
    }
    free(buf);
    buf = nullptr;
}

void client_on_readcb(struct bufferevent *bev, void *ctx) {
    struct timeval start_t;
    gettimeofday(&start_t, nullptr);

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
            a->m_callback->on_read(a, buf, len);
        } else {
            LOGI("no set callback server accept client");
        }
    }
    free(buf);
    buf = nullptr;
}

void server_on_accept(struct evconnlistener *listner, evutil_socket_t fd, struct sockaddr *addr,
                      int socklen, void *args) {
    LOGI("accept a client: fd:%d , socklen:%d, addr:%s", fd, socklen, addr->sa_data);
//    int enable = 1;
//    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable) < 0)) {
//        LOGI("TCP_NODELAY set error");
//    }
    if (args) {
        AutoTestInterface *a = (AutoTestInterface *) args;
        if (a->m_callback) {
            a->m_callback->on_connect(fd);
        } else {
            LOGI("no set callback server accept client: %d", fd);
        }
    }
    struct event_base *base = evconnlistener_get_base(listner);
    struct bufferevent *new_buff_event = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(new_buff_event, server_on_read, NULL, NULL, args);
    bufferevent_enable(new_buff_event, EV_READ | EV_WRITE);
}

void *start_server(void *args) {
    const char *ip = ((AutoTestInterface *) args)->hi.ip;
    unsigned int port = ((AutoTestInterface *) args)->hi.port;
    struct sockaddr_in remote_addr; //服务器端网络地址结构体
    memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零
    remote_addr.sin_family = AF_INET; //设置为IP通信
    remote_addr.sin_addr.s_addr = inet_addr(ip);//服务器IP地址
    remote_addr.sin_port = htons(port); //服务器端口号

    struct event_base *base = event_base_new();
    struct evconnlistener *listener = evconnlistener_new_bind(base, server_on_accept,
                                                              args,
                                                              LEV_OPT_CLOSE_ON_FREE |
                                                              LEV_OPT_REUSEABLE, -1,
                                                              (struct sockaddr *) &remote_addr,
                                                              sizeof(remote_addr));
    if (listener) {
        event_base_dispatch(base);
        evconnlistener_free(listener);
        event_base_free(base);
    } else {
        LOGI("create server error!");
    }
    return NULL;
}

void *start_client(void *args) {
    const char *ip = ((AutoTestInterface *) args)->hi.ip;
    unsigned int port = ((AutoTestInterface *) args)->hi.port;
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
    struct event_base *base = event_base_new();
    struct bufferevent *conn = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (bufferevent_socket_connect(conn, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) ==
        0) {
        if (args) {
            AutoTestInterface *a = (AutoTestInterface *) args;
            if (a->m_callback) {
                a->m_callback->on_connect(fd);
            } else {
                LOGI("no set callback server accept client: %d", fd);
            }
        }
        bufferevent_setcb(conn, client_on_readcb, NULL, NULL, args);
        bufferevent_enable(conn, EV_READ | EV_WRITE);
        event_base_dispatch(base);
        bufferevent_free(conn);
        event_base_free(base);
    } else {
        LOGI("============ connect failed! ");
    }
    return NULL;
}

int issetugid() {
    LOGI("============ issetugid ");
    return 0;
}

void arc4random_addrandom(unsigned char *c, int i) {
    LOGI("============ arc4random_addrandom: %s, %d", c, i);
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
        : m_inited(false), m_mode(0), m_callback(nullptr) {

}

AutoTestInterface::~AutoTestInterface() {

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
        m_callback->on_write(msg);
    } else {
        LOGI("no callback set for write");
    }
}
