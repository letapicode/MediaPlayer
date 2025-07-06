cmake_minimum_required(VERSION 3.18.1)
project(mediaplayer_android)

add_library(mediaplayer SHARED
    ../../core/src/AudioDecoder.cpp
    ../../core/src/VideoDecoder.cpp
    ../jni/MediaPlayerJNI.cpp
)

# Simplified: link to core library if built separately
find_library(log-lib log)

target_link_libraries(mediaplayer
    PUBLIC mediaplayer_core
    ${log-lib}
)
