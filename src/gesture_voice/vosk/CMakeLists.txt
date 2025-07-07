set(CMAKE_AUTOMOC ON)

add_library(mediaplayer_vosk
    src/VoskRecognizer.cpp
    src/VoiceInputQt.cpp
)

find_package(PkgConfig)
pkg_check_modules(VOSK REQUIRED IMPORTED_TARGET vosk)
find_package(Qt6 REQUIRED COMPONENTS Core Multimedia)

target_include_directories(mediaplayer_vosk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${VOSK_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_vosk
    Qt6::Core
    Qt6::Multimedia
    PkgConfig::VOSK
)

set_target_properties(mediaplayer_vosk PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
