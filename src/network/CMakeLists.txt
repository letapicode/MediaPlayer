add_library(mediaplayer_network
    src/NetworkStream.cpp
    src/ProtocolSupport.cpp
    src/HlsStream.cpp
    src/DashStream.cpp
    src/InternetRadioStream.cpp
    src/YouTubeDL.cpp
    src/upnp/DlnaClient.cpp
    src/remote/RemoteControlServer.cpp
    src/remote/RemoteControlClient.cpp
)

find_package(PkgConfig)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavformat)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

target_include_directories(mediaplayer_network PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${FFMPEG_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_network
    PkgConfig::FFMPEG
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Qt6::Network
)

set_target_properties(mediaplayer_network PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
