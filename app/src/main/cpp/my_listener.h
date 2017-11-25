#ifndef __my_listener_h__
#define __my_listener_h__

#include <string>
#include "auto_test_interface.h"

class MyCallback : public AutoTestInterfaceCallback {
public:
    virtual void on_read(AutoTestInterface *ctx, const char *msg, size_t len);

    virtual void on_connect(int fd);

    virtual void on_write(std::string msg);

private:
    int m_fd;
};

#endif// __my_listener_h__