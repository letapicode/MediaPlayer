cmake_minimum_required(VERSION 3.16)
project(MediaPlayer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/core)

add_executable(mediaplayer src/main.cpp)

target_link_libraries(mediaplayer PRIVATE mediaplayer_core)

target_compile_features(mediaplayer PRIVATE cxx_std_17)
