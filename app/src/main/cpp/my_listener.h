#ifndef __my_listener_h__
#define __my_listener_h__

#include <string>
#include "auto_test_interface.h"

class MyCallback : public AutoTestInterfaceCallback {
public:
    virtual void on_connect(AutoTestInterface *ctx, int fd);

    virtual void on_recv(AutoTestInterface *ctx, int fd, const char *msg, size_t len);

    virtual void on_error(AutoTestInterface *ctx, int code, int fd);

    virtual void on_write(AutoTestInterface *ctx, int fd,  const char *msg, size_t len);
};

#endif// __my_listener_h__