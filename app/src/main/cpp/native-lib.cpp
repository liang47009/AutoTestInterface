#include <jni.h>
#include <string>
#include "auto_test_interface.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

AutoTestInterface autotest_server;

AutoTestInterface autotest_client;

class MyCallback : AutoTestInterfaceCallback {

    virtual void on_read(void *sock, char *arg, size_t len) {

    }

    virtual void on_connect(int fd) {

    }
};

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_yunfeng_autotestinterface_JNILib_stringFromJNI(JNIEnv *env, jclass type) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_sendmsg(JNIEnv *env, jclass type, jstring text_) {
    const char *text = env->GetStringUTFChars(text_, 0);
    // TODO
    std::string msg(text);
    autotest_client.write_message(0, msg);
    env->ReleaseStringUTFChars(text_, text);
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_sendservermsg(JNIEnv *env, jclass type, jstring text_) {
    const char *text = env->GetStringUTFChars(text_, 0);
    std::string msg(text);
    autotest_server.write_message(0, msg);
    env->ReleaseStringUTFChars(text_, text);
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_startClient(JNIEnv *env, jclass type) {
    autotest_client.start("172.17.4.80", 5724, CLIENT_MODE);
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_startServer(JNIEnv *env, jclass type) {
    struct ifaddrs *ifaddrs1 = NULL;
    autotest_server.start("127.0.0.1", 8888, SERVER_MODE);
}

}