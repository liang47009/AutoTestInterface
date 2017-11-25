#ifndef __auto_test_interface_h__
#define __auto_test_interface_h__

enum {
    CLIENT_MODE, SERVER_MODE
};

struct host_info {
    const char *ip;
    unsigned int port;

    host_info() : ip(0), port(0) {}
};

class AutoTestInterfaceCallback {
public:
    virtual void on_read(AutoTestInterface *ctx, const char *msg, size_t len) {};

    virtual void on_connect(int fd) {};

    virtual void on_write(std::string msg) {}
};

class AutoTestInterface {

public:
    AutoTestInterface();

    ~AutoTestInterface();

    bool start(const char *ip, unsigned int port, int mode);

    void write_message(std::string msg);

    AutoTestInterfaceCallback *m_callback = nullptr;

    host_info hi;
private:
    bool m_inited;

    int m_mode;
};

#endif// __auto_test_interface_h__