
#include <string>
#include "sample.h"
#include "my_listener.h"

Sample::Sample()
        : autotest_client(nullptr), autotest_server(nullptr) {

}

Sample::~Sample() {
    if (autotest_client) {
        delete autotest_client;
        autotest_client = nullptr;
    }
    if (autotest_server) {
        delete autotest_server;
        autotest_server = nullptr;
    }
}

void Sample::initServer() {
    if (!autotest_server) {
        autotest_server = new AutoTestInterface;
        MyCallback *listener = new MyCallback;
        autotest_server->m_callback = listener;
        std::string ip = autotest_server->getLocalIPv4();
        autotest_server->start("0.0.0.0", 4999, SERVER_MODE);
    }
}

void Sample::initClient() {
    if (!autotest_client) {
        autotest_client = new AutoTestInterface;
        MyCallback *listener = new MyCallback;
        autotest_client->m_callback = listener;
        std::string ip = autotest_client->getLocalIPv4();
        autotest_client->start(ip.c_str(), 4999, CLIENT_MODE);
    }
}

void Sample::sendServermsg(std::string msg) {
    if (autotest_server) {
        autotest_server->write_message(msg.c_str(), msg.length());
//        delete autotest_server;
//        autotest_server = nullptr;
    }
}

void Sample::sendClientMsg(std::string msg) {
    if (autotest_client) {
        autotest_client->write_message(msg.c_str(), msg.length());
//        delete autotest_client;
//        autotest_client = nullptr;
    }
}
