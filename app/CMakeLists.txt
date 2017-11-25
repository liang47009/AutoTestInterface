# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/libevent/android)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( native-lib SHARED
            src/main/cpp/native-lib.cpp
            src/main/cpp/my_listener.cpp
            src/main/cpp/sample.cpp
            )

add_library( autotestinterface STATIC
            src/main/cpp/ifaddrs.c
            src/main/cpp/auto_test_interface.cpp )

add_library( errormode STATIC
            src/main/cpp/error.c)

add_library(eventextra STATIC IMPORTED)
set_target_properties(eventextra PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/src/main/cpp/libevent/lib/${ANDROID_ABI}/libeventextra.a)

add_library(eventcore STATIC IMPORTED)
set_target_properties(eventcore PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/src/main/cpp/libevent/lib/${ANDROID_ABI}/libeventcore.a)

target_link_libraries( native-lib log android autotestinterface eventextra eventcore errormode)