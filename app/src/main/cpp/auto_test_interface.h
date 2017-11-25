#ifndef __auto_test_interface_h__
#define __auto_test_interface_h__

#include <string>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

enum {
    CLIENT_MODE, SERVER_MODE
};

struct host_info {
    std::string ip;
    unsigned int port;

    host_info() : port(0) {}
};

class AutoTestInterface;

class AutoTestInterfaceCallback {

public:

    /**
     * 已连接
     * @param ctx
     * @param fd  sock id
     */
    virtual void on_connect(AutoTestInterface *ctx, int fd) {};

    /**
     * 收到消息
     * @param ctx
     * @param msg
     * @param len
     */
    virtual void on_recv(AutoTestInterface *ctx, const char *msg, size_t len) {};

    /**
     * 自定义写入内容
     * @param pInterface
     * @param msg
     * @param len
     */
    virtual void on_write(AutoTestInterface *pInterface, const char *msg, size_t len) {}

    /**
     * 出现错误
     * @param ctx
     * @param code
     * @param fd
     */
    virtual void on_error(AutoTestInterface *ctx, int code, int fd) {};

};

class AutoTestInterface {

public:
    AutoTestInterface();

    ~AutoTestInterface();

    /**
     * 启动一个接口
     * @param ip
     * @param port
     * @param mode
     * @return
     */
    bool start(const char *ip, unsigned int port, int mode);

    /**
     * 自定义写入内容
     * @param msg
     * @param len
     */
    void write_message(const char *msg, size_t len);

    /**
     * 获取本机地址 ipv4
     * @return
     */
    std::string getLocalIPv4();

    AutoTestInterfaceCallback *m_callback = nullptr;

private:
    bool m_inited;

    int m_mode;

public:
    host_info hi;
    bufferevent *conn;
    evconnlistener *m_baselistener;
    event_base *server_base;
    event_base *client_base;

};

#endif// __auto_test_interface_h__