#include <unistd.h>
#include "my_listener.h"
#include "android-log.h"

void MyCallback::on_error(AutoTestInterface *ctx, int code, int fd) {
    close(fd);
}

void MyCallback::on_recv(AutoTestInterface *ctx, int fd, const char *msg, size_t len) {
    LOGI("on_recv: fd: %d, length: %d, msg: %s", fd, len, msg);
}

void MyCallback::on_write(AutoTestInterface *ctx, int fd, const char *msg, size_t len) {
    write(fd, msg, len);
    LOGI("on_write: fd: %d, length: %d, msg: %s", fd, len, msg);
}

void MyCallback::on_connect(AutoTestInterface *ctx, int fd) {
    LOGI("on_connect fd: %d", fd);
}
