#include <unistd.h>
#include "my_listener.h"
#include "android-log.h"

void MyCallback::on_error(AutoTestInterface *ctx, int code, int fd) {
    close(fd);
}

void MyCallback::on_recv(AutoTestInterface *ctx, const char *msg, size_t len) {
    for (int i = 0; i < len; i++) {
        LOGI("on_recv msg: %d", msg[i]);
    }
}

void MyCallback::on_write(AutoTestInterface *ctx, const char *msg, size_t len) {
    LOGI("on_write msg:%s, %d", msg, len);
    write(fd, msg, len);
}

void MyCallback::on_connect(AutoTestInterface *ctx, int fd) {
    this->fd = fd;
}
