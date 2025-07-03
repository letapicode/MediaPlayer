add_library(mediaplayer_library
    src/LibraryDB.cpp
    src/RandomAIRecommender.cpp
    src/MoodAIRecommender.cpp
    src/Playlist.cpp
    src/LibraryScanner.cpp
    src/LibraryWorker.cpp
    src/LibraryFacade.cpp
)

find_package(PkgConfig)
pkg_check_modules(SQLITE3 REQUIRED IMPORTED_TARGET sqlite3)
pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavformat libavcodec libavutil)

target_include_directories(mediaplayer_library PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${SQLITE3_INCLUDE_DIRS}
    ${TAGLIB_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(mediaplayer_library
    PkgConfig::SQLITE3
    PkgConfig::TAGLIB
    PkgConfig::FFMPEG
)

set_target_properties(mediaplayer_library PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
