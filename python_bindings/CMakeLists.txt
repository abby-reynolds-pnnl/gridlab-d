cmake_minimum_required(VERSION 3.14)
project(gridlabd_python_bindings LANGUAGES CXX)

# Adjust path for nanobind (third_party relative to this file)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/third_party/nanobind
    ${CMAKE_BINARY_DIR}/third_party/nanobind
)
# You want to add your bindings library
add_library(gridlabd_python MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/gld_nanobind.cpp
    ${CMAKE_SOURCE_DIR}/gldcore/gldapi.cpp
)

# Include nanobind + the core header file directory
target_include_directories(gridlabd_python
    PRIVATE
        ${CMAKE_SOURCE_DIR}/third_party/nanobind/include
        ${CMAKE_SOURCE_DIR}/gldcore
)

#target_link_libraries(gridlabd_python PRIVATE nanobind::nanobind)

# Try to get Python's suffix (.pyd on windows)
set_target_properties(gridlabd_python PROPERTIES PREFIX "")
if (WIN32)
    set_target_properties(gridlabd_python PROPERTIES SUFFIX ".pyd")
else()
    set_target_properties(gridlabd_python PROPERTIES SUFFIX ".so")
endif()