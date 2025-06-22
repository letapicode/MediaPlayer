add_library(mediaplayer_subtitles
    src/SrtParser.cpp
)

target_include_directories(mediaplayer_subtitles PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(mediaplayer_subtitles PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
