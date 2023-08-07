cmake_minimum_required(VERSION 3.20.0)

set(BOARD sam_v71b_xult)
set(DTC_OVERLAY_FILE "boards/sam_v71b_xult.overlay")

find_package(Zephyr)

project(rovertito)

file(GLOB_RECURSE SOURCE_FILES "src/*.c" "lib/*.c")
target_sources(app PRIVATE ${SOURCE_FILES})

# Add the include directories from lib recursively
file(GLOB_RECURSE LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/lib/*/*.h")
foreach(header_file ${LIB_HEADERS})
    get_filename_component(dir ${header_file} DIRECTORY)
    list(APPEND LIB_INCLUDE_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES LIB_INCLUDE_DIRS)
include_directories(${LIB_INCLUDE_DIRS})

target_include_directories(app PRIVATE include
                                       src
                                       src/telecommands
                                       src/telemetries)
