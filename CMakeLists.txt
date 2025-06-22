cmake_minimum_required(VERSION 3.15) project(MediaPlayer LANGUAGES CXX)

    add_subdirectory(src / core)

        add_subdirectory(src / library)

#Format conversion utilities
            add_subdirectory(src / format_conversion)
#Optionally add other modules later
