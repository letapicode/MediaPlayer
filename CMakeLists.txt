cmake_minimum_required(VERSION 3.15)
project(MediaPlayer LANGUAGES CXX)

add_subdirectory(src/core)

add_subdirectory(src/library)

add_subdirectory(src/subtitles)

# Optionally add other modules later
