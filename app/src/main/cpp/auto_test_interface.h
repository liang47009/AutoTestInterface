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
    const char *ip;
    unsigned int port;

    host_info() : ip(0), port(0) {}
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
     * @param fd
     */
    virtual void on_recv(AutoTestInterface *ctx, std::string msg, int fd) {};

    /**
     * 出现错误
     * @param ctx
     * @param code
     * @param fd
     */
    virtual void on_error(AutoTestInterface *ctx, int code, int fd) {};

    /**
     * 自定义写入内容
     * @param pInterface
     * @param string
     */
    virtual void on_write(AutoTestInterface *pInterface, std::string msg) {}
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
     */
    void write_message(std::string msg);

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