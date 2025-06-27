set(CORE_SOURCES
    src/MediaPlayer.cpp
    src/AudioDecoder.cpp
    src/VideoDecoder.cpp
    src/PlaylistManager.cpp
    src/PacketQueue.cpp
    src/OpenGLVideoOutput.cpp
)

if(APPLE)
    list(APPEND CORE_SOURCES src/AudioOutputCoreAudio.mm)
elseif(UNIX)
    list(APPEND CORE_SOURCES src/AudioOutputPulse.cpp)
endif()

add_library(mediaplayer_core ${CORE_SOURCES})

find_package(PkgConfig)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libavutil
    libswresample
    libswscale
)
pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_include_directories(mediaplayer_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${FFMPEG_INCLUDE_DIRS}
    ${TAGLIB_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_core
    PkgConfig::FFMPEG
    PkgConfig::TAGLIB
    OpenGL::GL
    glfw
)

if(APPLE)
    target_link_libraries(mediaplayer_core
        "-framework AudioToolbox"
        "-framework CoreAudio")
else()
    find_library(PULSE_SIMPLE_LIB pulse-simple)
    find_library(PULSE_LIB pulse)
    target_link_libraries(mediaplayer_core
        ${PULSE_SIMPLE_LIB}
        ${PULSE_LIB})
endif()

set_target_properties(mediaplayer_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(mediaplayer_core PUBLIC MEDIAPLAYER_DESKTOP)
