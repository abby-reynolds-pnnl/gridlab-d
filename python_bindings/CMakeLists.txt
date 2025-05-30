cmake_minimum_required(VERSION 3.14)
project(gridlabd_python_bindings LANGUAGES CXX)


add_subdirectory(../nanobind nanobind_build_dir)

add_library(gridlabd_python MODULE
    gld_nanobind.cpp   
    gld_api.cpp     # Temporary location
)

target_include_directories(gridlabd_python PRIVATE ../ nanobind/include)
target_link_libraries(gridlabd_python PRIVATE nanobind::nanobind)

set_target_properties(gridlabd_python PROPERTIES PREFIX "")
if (WIN32)
    set_target_properties(gridlabd_python PROPERTIES SUFFIX ".pyd")
else()
    set_target_properties(gridlabd_python PROPERTIES SUFFIX ".so")
endif()