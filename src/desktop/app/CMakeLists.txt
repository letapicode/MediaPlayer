set(CMAKE_AUTOMOC ON)

    add_executable(
        mediaplayer_desktop_app main.cpp MediaPlayerController.cpp LibraryModel.cpp PlaylistModel
            .cpp NowPlayingModel.cpp SyncController.cpp AudioDevicesModel.cpp TranslationManager
            .cpp SettingsManager.cpp SmartPlaylistManager.cpp VideoItem.cpp MouseGestureFilter
            .cpp MicrophoneInput.cpp../
            ../ gesture_voice / VoskRecognizer.cpp../../ gesture_voice /
        VoiceCommandProcessor.cpp windows / WinIntegration.cpp windows / Hotkeys.cpp macos /
        MacIntegration.mm macos / TouchBar.mm linux / Mpris.cpp)

        find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2
                         Multimedia) find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
            find_package(PkgConfig) pkg_check_modules(VOSK REQUIRED IMPORTED_TARGET vosk) if (
                UNIX AND NOT APPLE) find_package(Qt6 REQUIRED COMPONENTS DBus) endif() if (WIN32)
                find_package(Qt6WinExtras) endif()

                    target_link_libraries(
                        mediaplayer_desktop_app PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                            Qt6::QuickControls2 Qt6::Multimedia Qt6::Gui Qt6::Widgets
                                mediaplayer_core mediaplayer_desktop mediaplayer_sync
                                    PkgConfig::VOSK) if (TARGET Qt6::DBus)
                        target_link_libraries(mediaplayer_desktop_app PRIVATE Qt6::DBus)
                            endif() if (TARGET Qt6::WinExtras) target_link_libraries(
                                mediaplayer_desktop_app PRIVATE Qt6::WinExtras) endif()

                                set_target_properties(mediaplayer_desktop_app PROPERTIES
                                                          CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

                                    qt_add_resources(
                                        mediaplayer_desktop_app_resources app
                                            .qrc) target_sources(mediaplayer_desktop_app PRIVATE ${
                                        mediaplayer_desktop_app_resources})

                                        target_include_directories(
                                            mediaplayer_desktop_app PRIVATE ${CMAKE_SOURCE_DIR} /
                                            src / core / include ${CMAKE_SOURCE_DIR} / src /
                                            core ${CMAKE_SOURCE_DIR} / src /
                                            desktop ${CMAKE_SOURCE_DIR} / src / gesture_voice)

                                            file(COPY qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
                                                file(COPY qml / PlaylistItemsDialog.qml qml /
                                                     PlaylistItemsView.qml qml /
                                                     SmartPlaylistEditor.qml DESTINATION ${
                                                         CMAKE_CURRENT_BINARY_DIR} /
                                                     qml) set(TS_FILES translations /
                                                              player_en.ts translations /
                                                              player_es.ts)
                                                    qt6_add_translations(QM_FILES ${TS_FILES})
                                                        add_custom_target(trans_qm ALL DEPENDS ${
                                                            QM_FILES})
                                                            add_dependencies(
                                                                mediaplayer_desktop_app trans_qm)
                                                                file(MAKE_DIRECTORY ${
                                                                         CMAKE_CURRENT_BINARY_DIR} /
                                                                     translations)
                                                                    file(
                                                                        COPY ${
                                                                            QM_FILES} DESTINATION ${
                                                                            CMAKE_CURRENT_BINARY_DIR} /
                                                                        translations)
                                                                        add_subdirectory(template)
