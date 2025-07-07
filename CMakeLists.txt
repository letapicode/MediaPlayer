cmake_minimum_required(VERSION 3.15)
project(MediaPlayer LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|AMD64|i[3-6]86)")
  set(SIMD_FLAGS "-msse2" "-mavx")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
  set(SIMD_FLAGS "-mfpu=neon")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  set(SIMD_FLAGS "-march=armv8-a+simd")
endif()

add_compile_options(${SIMD_FLAGS})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

add_subdirectory(src/core)

add_subdirectory(src/network)

add_subdirectory(src/library)

add_subdirectory(src/subtitles)

add_subdirectory(src/format_conversion)

add_subdirectory(src/desktop)
add_subdirectory(src/desktop/app)
# Sync module for cross-device playback position sharing
add_subdirectory(src/sync)

# Optionally add other modules later
add_subdirectory(src/visualization)

add_subdirectory(src/tools)
