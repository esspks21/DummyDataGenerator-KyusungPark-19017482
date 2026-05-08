cmake_minimum_required(VERSION 3.16)
project(DummyDataGenerator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    src/main.cpp
    src/DummyDataGenerator.cpp
    src/SimpleJsonParser.cpp
)

add_executable(DummyDataGenerator ${SOURCES})
target_include_directories(DummyDataGenerator PRIVATE src)

if(MSVC)
    target_compile_options(DummyDataGenerator PRIVATE /W4 /utf-8)
else()
    target_compile_options(DummyDataGenerator PRIVATE -Wall -Wextra -O2)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)

install(TARGETS DummyDataGenerator DESTINATION bin)
install(DIRECTORY data/ DESTINATION data)
