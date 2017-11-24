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
            src/main/cpp/auto_test_interface.cpp )

add_library( libautotestinterface STATIC
            src/main/cpp/auto_test_interface.cpp )

add_library(libeventextra STATIC IMPORTED)
set_target_properties(libeventextra PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/src/main/cpp/libevent/lib/${ANDROID_ABI}/libeventextra.a)

add_library(libeventcore STATIC IMPORTED)
set_target_properties(libeventcore PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/src/main/cpp/libevent/lib/${ANDROID_ABI}/libeventcore.a)

target_link_libraries( native-lib log android libautotestinterface libeventextra libeventcore)