cmake_minimum_required( VERSION 3.16 )

if ( DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE )
  set( CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "" )
endif()

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

project( lart
    VERSION ${PACKAGE_VERSION}
    LANGUAGES CXX
	DESCRIPTION "LLVM Single Context Library"
    HOMEPAGE_URL "https://github.com/xlauko/llvm-sc"
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi" )

include( cmake/project_settings.cmake )
include( cmake/prevent_in_source_builds.cmake )

add_library( sc_project_options INTERFACE )
target_compile_features( sc_project_options INTERFACE cxx_std_20 )
target_compile_options( sc_project_options INTERFACE "-fcoroutines" )

if ( CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )
    option( ENABLE_BUILD_WITH_TIME_TRACE
        "Enable -ftime-trace to generate time tracing .json files on clang" OFF )
    if ( ENABLE_BUILD_WITH_TIME_TRACE )
        add_compile_definitions( sc_project_options INTERFACE -ftime-trace )
    endif()
endif()

add_library( sc_project_warnings INTERFACE )

include( cmake/compiler_warnings.cmake )
set_project_warnings( sc_project_warnings )

# enable cache system
include( cmake/ccache.cmake )

include( cmake/sanitizers.cmake )

# allow for static analysis options
include( cmake/static_analyzers.cmake )

option( BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF )
option( ENABLE_TESTING "Enable Test Builds" ON )
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

set_property(TARGET llvmsc PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library( llvmsc::llvmsc ALIAS llvmsc )

target_link_libraries( llvmsc PRIVATE sc_project_options sc_project_warnings )
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
set( LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory" )

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

find_package( LLVM REQUIRED CONFIG )

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
