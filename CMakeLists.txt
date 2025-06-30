cmake_minimum_required(VERSION 3.15)
project(MediaPlayer LANGUAGES CXX)

add_subdirectory(src/core)

add_subdirectory(src/network)

add_subdirectory(src/library)

add_subdirectory(src/subtitles)

add_subdirectory(src/format_conversion)

add_subdirectory(src/desktop)

# Optionally add other modules later

add_subdirectory(src/tools)

option(BUILD_TESTS "Build test programs" OFF)

if(BUILD_TESTS)
    add_executable(network_stream_test tests/network_stream_test.cpp)
    target_link_libraries(network_stream_test
        mediaplayer_network
        PkgConfig::FFMPEG)
    if(TARGET PkgConfig::LIBCURL)
        target_link_libraries(network_stream_test PkgConfig::LIBCURL)
    endif()
endif()
