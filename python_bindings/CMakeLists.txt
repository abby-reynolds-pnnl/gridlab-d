cmake_minimum_required(VERSION 3.15)
project(gridlabd_python_bindings)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# Find and link GridLAB-D libraries
set(GRIDLABD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GRIDLABD_LIB_DIR ${GRIDLABD_ROOT}/cmake-build/lib)
set(GRIDLABD_INCLUDE_DIR ${GRIDLABD_ROOT}/gldcore)
set(JSONCPP_INCLUDE_DIR ${GRIDLABD_ROOT}/third_party/jsoncpp_lib/include)

# Check if GridLAB-D library exists
if(NOT EXISTS ${GRIDLABD_LIB_DIR}/libgldapi.so)
    message(FATAL_ERROR "GridLAB-D API library not found at ${GRIDLABD_LIB_DIR}/libgldapi.so. Please build GridLAB-D first.")
endif()

# Create the Python module
nanobind_add_module(
    _gridlabd_impl
    src_cpp/gld_nanobind.cpp
)

# Add include directories
target_include_directories(_gridlabd_impl PRIVATE 
    ${GRIDLABD_INCLUDE_DIR}
    ${GRIDLABD_ROOT}/third_party
    ${JSONCPP_INCLUDE_DIR}
)

# Link against GridLAB-D API library and dependencies
target_link_libraries(_gridlabd_impl PRIVATE 
    ${GRIDLABD_LIB_DIR}/libgldapi.so
    ${GRIDLABD_LIB_DIR}/static/libjsoncpp.a
)

# Set RPATH for the module to find the shared library
set_target_properties(_gridlabd_impl PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH FALSE
    INSTALL_RPATH "\$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Set module properties
target_compile_definitions(_gridlabd_impl PRIVATE VERSION_INFO=${SKBUILD_PROJECT_VERSION})

# Install the module and the shared library to the same directory
install(TARGETS _gridlabd_impl
    LIBRARY DESTINATION gridlabd
    RUNTIME DESTINATION gridlabd)

# Install the shared library alongside the module
install(FILES ${GRIDLABD_LIB_DIR}/libgldapi.so
        DESTINATION gridlabd)

# Create bundle directories
install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/gridlabd_bundle/bin)")
install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/gridlabd_bundle/lib)")

# Install additional libraries to bundle
set(GRIDLABD_MODULE_LIBS
    libgldcore.so
    powerflow.so
    residential.so
    commercial.so
    climate.so
    generators.so
    connection.so
    market.so
    reliability.so
    optimize.so
    tape.so
    tape_file.so
    tape_plot.so
    glxengine.so
    glsolvers.so
    mysql.so
    assert.so
    script.so
)

foreach(lib ${GRIDLABD_MODULE_LIBS})
    set(src "${GRIDLABD_LIB_DIR}/${lib}")
    if (EXISTS "${src}")
        install(FILES "${src}" DESTINATION gridlabd_bundle/lib)
    else()
        message(WARNING "Expected GridLAB-D module not found: ${src}")
    endif()
endforeach()