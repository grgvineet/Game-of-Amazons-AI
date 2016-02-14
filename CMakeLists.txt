cmake_minimum_required(VERSION 3.2)
project(BotChallange)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(Main ${SOURCE_FILES} Board.cpp Board.h Move.cpp Move.h constants.h Player.cpp Player.h AI.cpp AI.h)
