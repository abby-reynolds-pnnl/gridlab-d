cmake_minimum_required(VERSION 3.15...3.27)
project(gridlabd_bindings LANGUAGES CXX)

# Use C++17 for nanobind
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Python and nanobind
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module OPTIONAL_COMPONENTS Development.SABIModule)
find_package(nanobind CONFIG REQUIRED)

# Build a local static library from the API implementation
add_library(gldcore_local STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/cpp/gldcore/gldapi.cpp
)
set_target_properties(gldcore_local PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Headers live in cpp/gldcore
target_include_directories(gldcore_local PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/cpp/gldcore
)

# Build the nanobind extension
nanobind_add_module(
  _gridlabd_impl
  STABLE_ABI
  src/gld_nanobind.cpp
)

# Link the API into the Python extension
target_link_libraries(_gridlabd_impl PRIVATE gldcore_local)

# Install next to the Python package (Unix/macOS: LIBRARY; Windows: RUNTIME)
install(TARGETS _gridlabd_impl
  LIBRARY DESTINATION gridlabd
  RUNTIME DESTINATION gridlabd
)