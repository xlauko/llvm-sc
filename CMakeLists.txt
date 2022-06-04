cmake_minimum_required( VERSION 3.16 )

if ( DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE )
  set( CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "" )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set( SC_VERSION_MAJOR 0 )
set( SC_VERSION_MINOR 1 )
set( SC_VERSION_PATCH 0 )

if( NOT PACKAGE_VERSION )
  set( PACKAGE_VERSION "${SC_VERSION_MAJOR}.${SC_VERSION_MINOR}.${SC_VERSION_PATCH}" )
  if( DEFINED SC_VERSION_TWEAK )
    set( PACKAGE_VERSION "${PACKAGE_VERSION}-${SC_VERSION_TWEAK}" )
  endif()
  set( SC_VERSION_INFO ${PACKAGE_VERSION} )
endif()

project( llvm-sc
    LANGUAGES CXX
    VERSION 0.1.0
)

include( project_settings )
include( prevent_in_source_builds )

add_library( sc_project_options INTERFACE )
target_compile_features( sc_project_options INTERFACE cxx_std_20 )

add_library( sc_project_warnings INTERFACE )

include( compiler_warnings )
set_project_warnings( sc_project_warnings )

# enable cache system
include( ccache )

include( sanitizers )

# allow for static analysis options
include( static_analyzers )

option( BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF )
option( ENABLE_TESTING "Enable Test Builds" OFF )
option( ENABLE_FUZZING "Enable Fuzzing Builds" OFF )

option( ENABLE_PCH "Enable Precompiled Headers" OFF )
if ( ENABLE_PCH )
    target_precompile_headers(
        sc_project_options
      INTERFACE
        <vector>
        <string>
        <map>
        <utility>
    )
endif()

add_library( llvmsc
    src/annotation.cpp
    src/constant.cpp
    src/context.cpp
    src/init.cpp
    src/meta.cpp
    src/types.cpp
)

set_property( TARGET llvmsc PROPERTY POSITION_INDEPENDENT_CODE ON )
add_library( llvmsc::llvmsc ALIAS llvmsc )

find_package( cppcoro CONFIG REQUIRED )
find_package( range-v3 CONFIG REQUIRED )

target_link_libraries( llvmsc
    PUBLIC
        cppcoro
        range-v3
    PRIVATE
        sc_project_options
        sc_project_warnings
)

set_target_properties( llvmsc PROPERTIES CXX_EXTENSIONS OFF )

target_include_directories( llvmsc
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

message( STATUS "CMake generator: ${CMAKE_GENERATOR}" )

# VERIFY LLVM INSTALLATION DIR

# Set this to a valid LLVM installation dir
if ( DEFINED ENV{LLVM_INSTALL_DIR} AND NOT DEFINED LLVM_INSTALL_DIR )
  set( LLVM_INSTALL_DIR "$ENV{LLVM_INSTALL_DIR}" )
endif()

set( LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory" )
message( STATUS "Using LLVM_INSTALL_DIR: ${LLVM_INSTALL_DIR}" )

# A bit of a sanity checking
set( LLVM_INCLUDE_DIR "${LLVM_INSTALL_DIR}/include/llvm" )
if( NOT EXISTS "${LLVM_INCLUDE_DIR}" )
  message( FATAL_ERROR
    " LLVM_INCLUDE_DIR (${LLVM_INCLUDE_DIR}) is invalid." )
endif()

set( LLVM_CMAKE_FILE "${LLVM_INSTALL_DIR}/lib/cmake/llvm/LLVMConfig.cmake" )
if( NOT EXISTS "${LLVM_CMAKE_FILE}" )
  message(FATAL_ERROR
    " LLVM_CMAKE_FILE (${LLVM_CMAKE_FILE}) is invalid.")
endif()

# Add the location of LLVMConfig.cmake to CMake search paths (so that
# find_package can locate it)
list( APPEND CMAKE_PREFIX_PATH "${LLVM_INSTALL_DIR}/lib/cmake/llvm/" )

set (llvm_search_paths
  ${LLVM_INSTALL_DIR}
  ${LLVM_INSTALL_DIR}/lib/cmake
  ${LLVM_INSTALL_DIR}/lib/cmake/llvm
  ${LLVM_INSTALL_DIR}/lib/cmake/clang
  ${LLVM_INSTALL_DIR}/share/clang/cmake/
  ${LLVM_INSTALL_DIR}/share/llvm/cmake/
)

find_package( LLVM REQUIRED CONFIG
    PATHS ${llvm_search_paths}
    NO_DEFAULT_PATH
)

message( STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}" )
message( STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}" )

add_definitions( ${LLVM_DEFINITIONS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
include_directories( SYSTEM ${LLVM_INCLUDE_DIRS} )

llvm_map_components_to_libnames( llvm support core irreader )

if( NOT LLVM_ENABLE_RTTI )
  target_compile_options( llvmsc PUBLIC "-fno-rtti" )
endif()

# TESTS
if ( ENABLE_TESTING )
    enable_testing()
    add_subdirectory( test )
endif()
