
#include <string>
#include "sample.h"
#include "my_listener.h"

void Sample::initServer() {
    MyCallback* listener = new MyCallback;
    autotest_server.m_callback = listener;
    autotest_server.start("127.0.0.1", 8888, SERVER_MODE);
}

void Sample::initClient() {
    MyCallback* listener = new MyCallback;
    autotest_client.m_callback = listener;
    autotest_client.start("127.0.0.1", 8888, CLIENT_MODE);

}

void Sample::sendServermsg(std::string msg) {
    autotest_server.write_message(msg);
}

void Sample::sendClientMsg(std::string msg) {
    autotest_client.write_message(msg);
}
