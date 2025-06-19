add_library(mediaplayer_core
    src/MediaPlayer.cpp
    src/AudioDecoder.cpp
)

find_package(PkgConfig)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavformat libavcodec libavutil libswresample libswscale)

target_include_directories(mediaplayer_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_core
    PkgConfig::FFMPEG
)

set_target_properties(mediaplayer_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
