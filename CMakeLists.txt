cmake_minimum_required(VERSION 3.6)
project(EcoConsoleTPL)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        EcoConsoleTPL/Callbacks.h
        EcoConsoleTPL/Command.h
        EcoConsoleTPL/Converter.h
        EcoConsoleTPL/Invoke.h
        EcoConsoleTPL/Main.cpp
        EcoConsoleTPL/Utils.h)

add_executable(EcoConsoleTPL ${SOURCE_FILES})