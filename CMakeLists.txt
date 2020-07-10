cmake_minimum_required(VERSION 3.6)
project(donkeykong)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FILE(GLOB MyCSources *.cpp)
FILE(GLOB MyMedia Media/*)
set(SOURCE_FILES ${MyCSources})
add_executable(donkeykong ${MyCSources})

FILE(COPY ${MyMedia} DESTINATION "${CMAKE_BINARY_DIR}/Media")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML COMPONENTS system window graphics network audio)
if (SFML_FOUND)
    target_link_libraries(donkeykong PRIVATE sfml-graphics sfml-system sfml-window sfml-network sfml-audio)
endif()