#include <jni.h>
#include <string>
#include "auto_test_interface.h"
#include "sample.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

extern "C" {

Sample sample;

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
    sample.sendClientMsg(msg);
    env->ReleaseStringUTFChars(text_, text);
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_sendservermsg(JNIEnv *env, jclass type, jstring text_) {
    const char *text = env->GetStringUTFChars(text_, 0);
    std::string msg(text);
    sample.sendServermsg(msg);
    env->ReleaseStringUTFChars(text_, text);
}

void *initClient(void *args) {
    sample.initClient();
    return NULL;
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_startClient(JNIEnv *env, jclass type) {
    pthread_t m_pthread_t;
    int err = pthread_create(&m_pthread_t, NULL, initClient, NULL);
    pthread_detach(m_pthread_t);
}

JNIEXPORT void JNICALL
Java_com_yunfeng_autotestinterface_JNILib_startServer(JNIEnv *env, jclass type) {
    sample.initServer();
}

}