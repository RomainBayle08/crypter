cmake_minimum_required(VERSION 3.28)
project(crypter)

set(CMAKE_CXX_STANDARD 17)

add_executable(crypter main.cpp
        crypter.cpp
        crypter.h
        AES_256.cpp
        AES_256.hpp)


