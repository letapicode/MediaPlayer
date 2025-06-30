add_library(mediaplayer_network
    src/NetworkStream.cpp
    src/YouTubeResolver.cpp
)

find_package(PkgConfig)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavformat)
find_package(Python3 COMPONENTS Interpreter)

target_include_directories(mediaplayer_network PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_network PkgConfig::FFMPEG)

if(Python3_FOUND)
    target_compile_definitions(mediaplayer_network PRIVATE
        YOUTUBE_RESOLVER_SCRIPT="${CMAKE_INSTALL_FULL_BINDIR}/youtube_resolver.py")
endif()

set_target_properties(mediaplayer_network PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS mediaplayer_network DESTINATION lib)
install(PROGRAMS ../tools/youtube_resolver.py DESTINATION bin)
