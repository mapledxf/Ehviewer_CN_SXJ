cmake_minimum_required(VERSION 3.18.1)

project("encryption-jni")

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "Android")
    add_library(encryption-jni SHARED
            encryption-jni.cpp)
    target_link_libraries(encryption-jni log)
endif()
add_executable(decrypt decrypt_main.cc)
add_executable(encrypt encrypt_main.cc)
