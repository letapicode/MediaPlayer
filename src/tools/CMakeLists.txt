add_executable(mediaconvert mediaconvert.cpp)

target_link_libraries(mediaconvert PRIVATE mediaplayer_conversion)

set_target_properties(mediaconvert PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
