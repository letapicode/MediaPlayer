file(GLOB_RECURSE TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*_test.cpp")

set(COMMON_LIBS
    mediaplayer_core
    mediaplayer_library
    mediaplayer_network
    mediaplayer_conversion
    mediaplayer_subtitles
    mediaplayer_sync
    mediaplayer_visualization
    Qt6::Core
)

include(GoogleTest)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${test_src} contents)
    if(contents MATCHES "gtest")
        target_link_libraries(${test_name} PRIVATE
            ${COMMON_LIBS}
            gtest_main
        )
        gtest_discover_tests(${test_name})
    else()
        target_link_libraries(${test_name} PRIVATE
            ${COMMON_LIBS}
        )
    endif()
endforeach()
