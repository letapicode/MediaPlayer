set(CMAKE_AUTOMOC ON)

add_library(mediaplayer_desktop
    FormatConverterQt.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core)

target_include_directories(mediaplayer_desktop PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/format_conversion/include
)

target_link_libraries(mediaplayer_desktop PUBLIC Qt6::Core mediaplayer_conversion)

set_target_properties(mediaplayer_desktop PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
