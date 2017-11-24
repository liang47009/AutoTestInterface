#ifndef __auto_test_interface_h__
#define __auto_test_interface_h__

enum {
    CLIENT_MODE, SERVER_MODE
};

class AutoTestInterfaceCallback {
public:
    virtual void on_read(void *sock, char *arg, size_t len) {}

    virtual void on_connect(int fd) {}
private:
    int m_fd;
};

class AutoTestInterface {

public:
    AutoTestInterface();

    ~AutoTestInterface();

    void setCallback(AutoTestInterfaceCallback *m_callback);

    bool start(const char *ip, unsigned int port, int mode);

    void write_message(const char *msg);

    void write_message(const std::string msg);

private:
    bool m_inited;

    int m_mode;
};

#endif// __auto_test_interface_h__