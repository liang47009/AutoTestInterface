
#ifndef ANDROID_LOG__H
#define ANDROID_LOG__H

#include <android/log.h>

#define  LOG_TAG    "Auto_Test_Interface"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif      // ANDROID_LOG__H
