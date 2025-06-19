add_library(mediaplayer_core
    src/MediaPlayer.cpp
)

target_include_directories(mediaplayer_core PUBLIC include)

target_compile_features(mediaplayer_core PUBLIC cxx_std_17)
