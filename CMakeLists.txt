cmake_minimum_required(VERSION 3.1)
project(Gandiv)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

add_executable(Gandiv
        src/main.cpp
        src/lexer.h
        src/lexer.cpp
        src/parser.h
        src/parser.cpp
        src/interpreter.h
        src/interpreter.cpp)
