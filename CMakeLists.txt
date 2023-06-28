cmake_minimum_required(VERSION 3.25)
project(road_sim)

set(CMAKE_CXX_STANDARD 17)

add_executable(road_sim main.cpp olcPixelGameEngine.h olcSoundWaveEngine.h)

#include_directories(music)
include_directories(sprites)

target_link_libraries(road_sim pulse pulse-simple X11 GL pthread png  "stdc++fs")
