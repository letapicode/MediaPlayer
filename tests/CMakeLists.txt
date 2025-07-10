# Test suite for MediaPlayer

# The tests are simple executables using assert() for validation.
# Each executable is registered with CTest so `ctest` runs the full suite.

# Subtitles tests
add_executable(test_srtparser test_srtparser.cpp)
target_link_libraries(test_srtparser PRIVATE mediaplayer_subtitles)
add_test(NAME test_srtparser COMMAND test_srtparser)

add_executable(subtitle_provider_test subtitle_provider_test.cpp)
target_link_libraries(subtitle_provider_test PRIVATE mediaplayer_subtitles)
add_test(NAME subtitle_provider_test COMMAND subtitle_provider_test)

add_executable(subtitle_decoder_test subtitle_decoder_test.cpp)
target_link_libraries(subtitle_decoder_test PRIVATE mediaplayer_core)
add_test(NAME subtitle_decoder_test COMMAND subtitle_decoder_test)

# Format conversion tests
add_executable(format_conversion_test format_conversion_test.cpp)
target_link_libraries(format_conversion_test PRIVATE mediaplayer_conversion)
add_test(NAME format_conversion_test COMMAND format_conversion_test)

add_executable(video_conversion_test video_conversion_test.cpp)
target_link_libraries(video_conversion_test PRIVATE mediaplayer_conversion)
add_test(NAME video_conversion_test COMMAND video_conversion_test)

# Media library tests
set(LIBRARY_TESTS
    library_playlist_test
    library_autoplaylist_test
    library_db_update_test
    library_playback_update_test
    library_rating_test
    library_search_test
    library_smartplaylist_test
    library_cleanup_test
    library_purge_removed_test
    library_recommender_test
    library_rescan_update_test
    library_ftssearch_test
    library_ignore_nonmedia_test
    library_invalid_filter_test
    library_smartquery_test
    library_video_metadata_test
    library_video_notag_index_test
)
foreach(t ${LIBRARY_TESTS})
    add_executable(${t} ${t}.cpp)
    target_link_libraries(${t} PRIVATE mediaplayer_library)
    add_test(NAME ${t} COMMAND ${t})
endforeach()

# Networking and sync tests
add_executable(remote_control_test remote_control_test.cpp)
target_link_libraries(remote_control_test PRIVATE Qt6::Core Qt6::Network mediaplayer_network)
add_test(NAME remote_control_test COMMAND remote_control_test)

add_executable(mdns_service_test mdns_service_test.cpp)
target_link_libraries(mdns_service_test PRIVATE mediaplayer_sync)
add_test(NAME mdns_service_test COMMAND mdns_service_test)

add_executable(upnp_enum_test upnp_enum_test.cpp)
target_link_libraries(upnp_enum_test PRIVATE mediaplayer_network)
add_test(NAME upnp_enum_test COMMAND upnp_enum_test)

# Core stress test
add_executable(stress_load_test stress_load_test.cpp)
target_link_libraries(stress_load_test PRIVATE mediaplayer_core)
add_test(NAME stress_load_test COMMAND stress_load_test)
