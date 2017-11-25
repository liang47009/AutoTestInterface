#include <unistd.h>
#include "my_listener.h"
#include "android-log.h"

void MyCallback::on_connect(int fd) {
    this->m_fd = fd;
}

void MyCallback::on_write(std::string msg) {
    write(m_fd, msg.c_str(), msg.length());
}

void MyCallback::on_read(AutoTestInterface *ctx, const char *msg, size_t len) {
    std::string text(msg);
    LOGI("read from: msg:%s", text.c_str());
}
