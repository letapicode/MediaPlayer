set(CMAKE_AUTOMOC ON)

add_executable(mediaplayer_desktop_app
    main.cpp
    MediaPlayerController.cpp
    LibraryModel.cpp
    PlaylistModel.cpp
    VideoItem.cpp
    windows/WinIntegration.cpp
    macos/MacIntegration.mm
    linux/Mpris.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
if(UNIX AND NOT APPLE)
    find_package(Qt6 REQUIRED COMPONENTS DBus)
endif()
if(WIN32)
    find_package(Qt6WinExtras)
endif()

target_link_libraries(mediaplayer_desktop_app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Gui
    Qt6::Widgets
    mediaplayer_core
    mediaplayer_desktop
)
if(TARGET Qt6::DBus)
    target_link_libraries(mediaplayer_desktop_app PRIVATE Qt6::DBus)
endif()
if(TARGET Qt6::WinExtras)
    target_link_libraries(mediaplayer_desktop_app PRIVATE Qt6::WinExtras)
endif()

set_target_properties(mediaplayer_desktop_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

qt_add_resources(mediaplayer_desktop_app_resources app.qrc)
target_sources(mediaplayer_desktop_app PRIVATE ${mediaplayer_desktop_app_resources})

target_include_directories(mediaplayer_desktop_app PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/desktop
)

file(COPY qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY translations DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
