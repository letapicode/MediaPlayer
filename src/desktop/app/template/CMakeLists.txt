set(CMAKE_AUTOMOC ON)

add_executable(template_app
    main.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

target_link_libraries(template_app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

file(COPY qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
