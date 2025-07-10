find_package(GTest REQUIRED)

add_executable(core_unit_tests
    core_unit_tests.cpp)

target_link_libraries(core_unit_tests
    GTest::gtest_main
    mediaplayer_core
    mediaplayer_conversion)

set_target_properties(core_unit_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)
