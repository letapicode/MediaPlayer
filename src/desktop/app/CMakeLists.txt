set(CMAKE_AUTOMOC ON)

add_executable(mediaplayer_desktop_app
    main.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

target_link_libraries(mediaplayer_desktop_app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    mediaplayer_core
    mediaplayer_desktop
)

set_target_properties(mediaplayer_desktop_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(mediaplayer_desktop_app PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/desktop
)

file(COPY qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
