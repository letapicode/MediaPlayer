add_library(mediaplayer_conversion src / AudioConverter.cpp)

    find_package(PkgConfig) pkg_check_modules(
        FFMPEG REQUIRED IMPORTED_TARGET libavformat libavcodec libavutil libswresample)

        target_include_directories(
            mediaplayer_conversion PUBLIC $<BUILD_INTERFACE : ${CMAKE_CURRENT_SOURCE_DIR} / include>
                $<INSTALL_INTERFACE : include>
                    ${FFMPEG_INCLUDE_DIRS})

            target_link_libraries(mediaplayer_conversion PkgConfig::FFMPEG)

                set_target_properties(
                    mediaplayer_conversion PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
