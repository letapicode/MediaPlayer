set(CMAKE_AUTOMOC ON)

add_library(mediaplayer_desktop
    FormatConverterQt.cpp
    LibraryQt.cpp
    AudioOutputQt.cpp
    VisualizerQt.cpp
    VisualizerItem.cpp
    VideoOutputQt.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

target_include_directories(mediaplayer_desktop PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/format_conversion/include
    ${CMAKE_SOURCE_DIR}/src/library/include
    ${CMAKE_SOURCE_DIR}/src/visualization/include
)

target_link_libraries(mediaplayer_desktop PUBLIC
    Qt6::Core Qt6::Qml Qt6::Quick
    mediaplayer_conversion
    mediaplayer_library
    mediaplayer_visualization
)

set_target_properties(mediaplayer_desktop PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
