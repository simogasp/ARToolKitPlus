cmake_minimum_required(VERSION 3.20)
project(ARToolKitPlus
        VERSION 2.3.0
        LANGUAGES CXX
        DESCRIPTION "ARToolKitPlus - A C++ based Augmented Reality Tracking Library"
        )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

option(ARTK_BUILD_EXAMPLES "Build sample programs and utilies" ON)
option(ARTK_USE_BOOST "Compile with boost (OpenCV calibration file support)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ARTK_ENABLE_WARNINGS_AS_ERRORS "Enable warnings as errors" OFF)

message(STATUS "ARToolKitPlus version: ${PROJECT_VERSION}")
message(STATUS "Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "Build examples: ${ARTK_BUILD_EXAMPLES}")
message(STATUS "Treat warnings as errors: ${ARTK_ENABLE_WARNINGS_AS_ERRORS}")

# compile options for all targets depending on the compiler and the system
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(ARTK_COMPILE_OPTIONS "-Wall;-Wextra;-pedantic;-Wno-comment;-Wshadow;-Wnon-virtual-dtor;-Wpedantic;-Wconversion;-Wmisleading-indentation;-Wsign-conversion;-Wfloat-equal")
    if(ARTK_ENABLE_WARNINGS_AS_ERRORS)
        list(APPEND ARTK_COMPILE_OPTIONS "-Werror")
    endif()
elseif(MSVC)
    set(ARTK_COMPILE_OPTIONS "/W4;/permissive-")
    if(ARTK_ENABLE_WARNINGS_AS_ERRORS)
        list(APPEND ARTK_COMPILE_OPTIONS "/WX")
    endif()
endif()
if(APPLE)
    list(APPEND ARTK_COMPILE_OPTIONS "-Wno-deprecated-declarations")
endif()

# compile definitions for all targets depending on the compiler and the system
set(ARTK_COMPILE_DEFINITIONS "")
if(MSVC)
    list(APPEND ARTK_COMPILE_DEFINITIONS "-DNOMINMAX;-D_USE_MATH_DEFINES;-D_CRT_SECURE_NO_WARNINGS;-DAR_LIBRARY")
endif()


set(ARTK_CXX_STANDARD 11)
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD ${ARTK_CXX_STANDARD})
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(CMAKE_CXX_STANDARD LESS ARTK_CXX_STANDARD)
  message(FATAL_ERROR "CMake CXX standard (${CMAKE_CXX_STANDARD}) is less than the required standard (${ARTK_CXX_STANDARD})")
endif()
set(ARTK_CXX_FEATURE cxx_std_${CMAKE_CXX_STANDARD})
message(STATUS "Using C++ standard: ${CMAKE_CXX_STANDARD}")


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(ARTK_USE_BOOST)
    add_definitions(-DAR_WITH_BOOST)
endif(ARTK_USE_BOOST)

set(AR_SOURCE_DIR ${PROJECT_SOURCE_DIR})

# ARToolkitPlus core files (headers and sources)
file(GLOB_RECURSE HEADERS_AR ${AR_SOURCE_DIR}/include/ARToolKitPlus/*.h)
file(GLOB SOURCES_AR
	${AR_SOURCE_DIR}/src/*.cpp
	${AR_SOURCE_DIR}/src/core/*.cpp
	${AR_SOURCE_DIR}/src/extra/*.cpp
	${AR_SOURCE_DIR}/src/librpp/*.cpp
	)

add_library(ARToolKitPlus SHARED ${HEADERS_AR} ${SOURCES_AR})
target_compile_options(ARToolKitPlus PRIVATE ${ARTK_COMPILE_OPTIONS})
target_compile_definitions(ARToolKitPlus PRIVATE ${ARTK_COMPILE_DEFINITIONS})
target_include_directories(ARToolKitPlus PUBLIC
  "$<BUILD_INTERFACE:${AR_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/>")
target_compile_features(ARToolKitPlus PUBLIC ${ARTK_CXX_FEATURE})

if(ARTK_BUILD_EXAMPLES)
    add_subdirectory(bin/)
endif(ARTK_BUILD_EXAMPLES)

# make install
install(TARGETS ARToolKitPlus 
LIBRARY DESTINATION lib  
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)

