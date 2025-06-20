add_library(mediaplayer_library
    src/LibraryDB.cpp
)

find_package(PkgConfig)
pkg_check_modules(SQLITE3 REQUIRED IMPORTED_TARGET sqlite3)
pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib)

target_include_directories(mediaplayer_library PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${SQLITE3_INCLUDE_DIRS}
    ${TAGLIB_INCLUDE_DIRS}
)

target_link_libraries(mediaplayer_library
    PkgConfig::SQLITE3
    PkgConfig::TAGLIB
)

set_target_properties(mediaplayer_library PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
