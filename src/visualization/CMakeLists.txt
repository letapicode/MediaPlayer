add_library(mediaplayer_visualization
    src/BasicVisualizer.cpp
)

target_include_directories(mediaplayer_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(mediaplayer_visualization
    mediaplayer_core
)

set_target_properties(mediaplayer_visualization PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
