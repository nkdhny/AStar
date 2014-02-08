project(fifteenastar)
cmake_minimum_required(VERSION 2.8)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++11")

enable_testing()

add_executable(modeltest modeltest.cpp)
target_link_libraries(modeltest gtest pthread)
add_test(model modeltest)

add_executable(astartest astarfifteentest.cpp)
target_link_libraries(astartest gtest pthread)
add_test(astar astartesttest)
