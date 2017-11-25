#include <unistd.h>
#include "my_listener.h"
#include "android-log.h"

void MyCallback::on_error(AutoTestInterface *ctx, int code, int fd) {
    close(fd);
}

void MyCallback::on_recv(AutoTestInterface *ctx, std::string msg, int fd) {
    LOGI("read from: msg:%s", msg.c_str());
}

void MyCallback::on_write(AutoTestInterface *ctx, std::string msg) {
    write(fd, msg.c_str(), msg.length());
}

void MyCallback::on_connect(AutoTestInterface *ctx, int fd) {
    this->fd = fd;
}
