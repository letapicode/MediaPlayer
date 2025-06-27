add_library(mediaplayer_core
    src/MediaPlayer.cpp
    src/AudioDecoder.cpp
    src/VideoDecoder.cpp
    src/PlaylistManager.cpp
    src/PacketQueue.cpp
    src/OpenGLVideoOutput.cpp
)

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

if (WIN32)
  target_sources(mediaplayer_core PRIVATE src/AudioOutputWASAPI.cpp)
  target_link_libraries(mediaplayer_core ole32)
else()
  target_sources(mediaplayer_core PRIVATE src/AudioOutputPulse.cpp)
  find_library(PULSE_SIMPLE_LIB pulse-simple)
  find_library(PULSE_LIB pulse)
  target_link_libraries(mediaplayer_core ${PULSE_SIMPLE_LIB} ${PULSE_LIB})
endif()



set_target_properties(mediaplayer_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(mediaplayer_core PUBLIC MEDIAPLAYER_DESKTOP)
