cmake_minimum_required(VERSION 3.16)

project(VC_lab_02_aa25-26 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SNITCH_DEFINE_MAIN OFF)

# Add snitch test framework to the project
include(FetchContent)

FetchContent_Declare(snitch
        GIT_REPOSITORY https://github.com/snitch-org/snitch.git
        GIT_TAG v1.3.2) # update version number as needed
FetchContent_MakeAvailable(snitch)

# Add Eigen include path
set(EIGEN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(srimg++ SHARED # stands for "San Raffaele Image Library ++"
        src/utils.cpp
        src/utils.h
        src/image.h
        src/load_image.cpp
        src/stb_image.h
        src/stb_image_write.h
        src/test/test0_legacy.hpp
        src/test/test1_legacy.hpp
        src/access_image.cpp # the file edited in the previous lab
        src/process_image.cpp # the file edited in the previous lab

        src/resize_image.cpp  # file to edit
        src/filter_image.cpp

)

target_include_directories(srimg++ PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${EIGEN_INCLUDE_DIR}
)

add_executable(test0 src/test/test0.cpp)
target_link_libraries(test0 PRIVATE srimg++ snitch::snitch)
add_executable(test1 src/test/test1.cpp)
target_link_libraries(test1 PRIVATE srimg++ snitch::snitch)


