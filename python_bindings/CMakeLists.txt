cmake_minimum_required(VERSION 3.14)
project(gridlabd_python LANGUAGES CXX)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

# Add nanobind submodule and build static library
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/third_party/nanobind
    ${CMAKE_BINARY_DIR}/third_party/nanobind
)
nanobind_build_library(nanobind)

add_library(gridlabd_py MODULE
    gld_nanobind.cpp
    ${CMAKE_SOURCE_DIR}/gldcore/gldapi.cpp
)

target_include_directories(gridlabd_py PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/nanobind/include
    ${CMAKE_SOURCE_DIR}/gldcore
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(gridlabd_py PRIVATE nanobind ${Python3_LIBRARIES})

# This is KEY: output the .so to the source package dir!
set_target_properties(gridlabd_py PROPERTIES
    PREFIX ""
    OUTPUT_NAME "gridlabd"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/pylib/gridlabd"
    SUFFIX ".so"
)

# Make sure the output dir exists before build (important for clean tree builds)
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/pylib/gridlabd")

# If you want to be *extra* sure, you can also add a post-build copy (redundant if you get the output dir right):
add_custom_command(TARGET gridlabd_py POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         $<TARGET_FILE:gridlabd_py>
         ${CMAKE_SOURCE_DIR}/pylib/gridlabd/
)
