add_library(mediaplayer_network
    src/NetworkStream.cpp
    src/YoutubeResolver.cpp
)

find_package(PkgConfig)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavformat)

target_include_directories(mediaplayer_network PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_network PkgConfig::FFMPEG)

set_target_properties(mediaplayer_network PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
