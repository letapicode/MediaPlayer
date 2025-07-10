add_executable(format_converter_test format_converter_test.cpp)
    target_link_libraries(format_converter_test PRIVATE mediaplayer_conversion gtest_main)

        add_executable(library_db_test library_db_test.cpp)
            target_link_libraries(library_db_test PRIVATE mediaplayer_library gtest_main)

                add_executable(visualizer_test visualizer_test.cpp)
                    target_link_libraries(visualizer_test PRIVATE mediaplayer_core gtest_main)

                        add_executable(playback_integration_test playback_integration_test.cpp)
                            target_link_libraries(
                                playback_integration_test PRIVATE mediaplayer_core gtest_main)

                                include(GoogleTest) gtest_discover_tests(format_converter_test)
                                    gtest_discover_tests(library_db_test)
                                        gtest_discover_tests(visualizer_test)
                                            gtest_discover_tests(playback_integration_test)
                                            add_executable(startup_time_test startup_time_test.cpp)
                                                target_link_libraries(startup_time_test PRIVATE mediaplayer_core gtest_main)
                                            gtest_discover_tests(startup_time_test)
                                            add_executable(format_probe format_probe.cpp)
                                                target_link_libraries(format_probe PRIVATE mediaplayer_core)
