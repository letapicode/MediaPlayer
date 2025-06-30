set(CORE_SOURCES src / MediaPlayer.cpp src / AudioDecoder.cpp src / VideoDecoder.cpp src /
    PlaylistManager.cpp src / PacketQueue.cpp src / MediaDemuxer.cpp src / SubtitleDecoder.cpp src /
    BufferedReader.cpp src / OpenGLVideoOutput.cpp src / RingBuffer.cpp src / VideoFrameQueue.cpp)

    if (CMAKE_SYSTEM_NAME STREQUAL
        "iOS") list(APPEND CORE_SOURCES src / AudioOutputiOS.mm src /
                    MetalVideoOutput.mm) elseif(APPLE) list(APPEND CORE_SOURCES src /
                                                            AudioOutputCoreAudio.mm src /
                                                            MetalVideoOutput.mm) elseif(WIN32)
        list(
            APPEND CORE_SOURCES src / AudioOutputWASAPI.cpp src /
            Direct3D11VideoOutput
                .cpp) elseif(ANDROID) list(APPEND CORE_SOURCES src / AudioOutputAndroid.cpp../
                                           android /
                                           AndroidGLVideoOutput
                                               .cpp) elseif(UNIX) list(APPEND CORE_SOURCES src /
                                                                       AudioOutputPulse.cpp) endif()

#Hardware decoding requires FFmpeg to be built with `--enable - hwaccels` and
#the relevant device backends(e.g. `--enable - vaapi`).
            option(ENABLE_HW_DECODING "Enable hardware accelerated video decoding" ON)

                add_library(mediaplayer_core ${CORE_SOURCES})

                    find_package(PkgConfig) pkg_check_modules(
                        FFMPEG REQUIRED IMPORTED_TARGET libavformat libavcodec libavutil
                            libswresample libswscale)
                        pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib) find_package(
                            OpenGL REQUIRED) find_package(glfw3 REQUIRED)

                            target_include_directories(
                                mediaplayer_core PUBLIC
                                        $<BUILD_INTERFACE : ${CMAKE_CURRENT_SOURCE_DIR} / include>
                                            $<INSTALL_INTERFACE : include>
                                                ${FFMPEG_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS} ${
                                                    CMAKE_SOURCE_DIR} /
                                    src / library / include / mediaplayer ${CMAKE_SOURCE_DIR} /
                                    src / subtitles / include $ <
                                $ < PLATFORM_ID : Android > : ${CMAKE_SOURCE_DIR} / src / android >)

                                target_link_libraries(
                                    mediaplayer_core PkgConfig::FFMPEG PkgConfig::TAGLIB
                                        mediaplayer_network mediaplayer_conversion
                                            mediaplayer_subtitles OpenGL::GL glfw)

                                    if (ENABLE_HW_DECODING) if (WIN32) target_link_libraries(
                                        mediaplayer_core dxva2 d3d11
                                            dxgi) elseif(APPLE) target_link_libraries(mediaplayer_core
                                                                                      "-framework "
                                                                                      "VideoToolbox"
                                                                                      "-framework "
                                                                                      "CoreVideo")
                                        elseif(ANDROID) find_library(ANDROID_LOG_LIB log) find_library(
                                            JNIGRAPHICS_LIB
                                                jnigraphics) target_link_libraries(mediaplayer_core ${
                                            ANDROID_LOG_LIB} ${
                                            JNIGRAPHICS_LIB}) elseif(UNIX) pkg_check_modules(LIBVA REQUIRED
                                                                                                 IMPORTED_TARGET
                                                                                                     libva)
                                            target_link_libraries(
                                                mediaplayer_core PkgConfig::LIBVA) endif() endif()

                                                if (CMAKE_SYSTEM_NAME STREQUAL
                                                    "iOS") target_link_libraries(mediaplayer_core
                                                                                 "-framework "
                                                                                 "AVFoundation"
                                                                                 "-framework "
                                                                                 "AudioToolbox"
                                                                                 "-framework Metal"
                                                                                 "-framework "
                                                                                 "QuartzCore"
                                                                                 "-framework "
                                                                                 "MetalKit")
                                                    elseif(APPLE) target_link_libraries(
                                                        mediaplayer_core "-framework AudioToolbox"
                                                                         "-framework CoreAudio"
                                                                         "-framework Metal"
                                                                         "-framework QuartzCore"
                                                                         "-framework MetalKit")
                                                        elseif(WIN32) target_link_libraries(
                                                            mediaplayer_core ole32 d3d11 dxgi
                                                                dxguid) elseif(ANDROID)
                                                            find_library(OPENSL_LIB OpenSLES) find_library(
                                                                AAUDIO_LIB
                                                                    aaudio) find_library(ANDROID_LOG_LIB log)
                                                                target_link_libraries(mediaplayer_core ${
                                                                    OPENSL_LIB} ${AAUDIO_LIB} ${
                                                                    ANDROID_LOG_LIB}) else()
                                                                    find_library(
                                                                        PULSE_SIMPLE_LIB
                                                                            pulse -
                                                                        simple) find_library(PULSE_LIB pulse)
                                                                        target_link_libraries(
                                                                            mediaplayer_core ${
                                                                                PULSE_SIMPLE_LIB} ${
                                                                                PULSE_LIB}) endif()

                                                                            set_target_properties(
                                                                                mediaplayer_core PROPERTIES
                                                                                    CXX_STANDARD 17 CXX_STANDARD_REQUIRED
                                                                                        ON)

                                                                                target_compile_definitions(
                                                                                    mediaplayer_core PUBLIC MEDIAPLAYER_DESKTOP
                                                                                        $<$<BOOL : ${
                                                                                            ENABLE_HW_DECODING}> : MEDIAPLAYER_HW_DECODING>)
