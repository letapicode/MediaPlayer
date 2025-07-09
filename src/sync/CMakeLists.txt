add_library(mediaplayer_sync
    src/SyncService.cpp
    src/mdns/MdnsService.cpp
    src/cloud/CloudSyncService.cpp
    ../thirdparty/mdns/mdns.c
)

set_target_properties(mediaplayer_sync PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(mediaplayer_sync PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/mdns
)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(mediaplayer_sync
    CURL::libcurl
    nlohmann_json::nlohmann_json
)
