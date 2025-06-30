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
