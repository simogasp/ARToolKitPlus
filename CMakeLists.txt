cmake_minimum_required(VERSION 3.20)
project(ARToolKitPlus
        VERSION 2.2.1
        LANGUAGES CXX
        DESCRIPTION "ARToolKitPlus - A C++ based Augmented Reality Tracking Library"
        )

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(ARTK_IS_MAIN_PROJECT ON)
    set(ARTK_INSTALL_DEFAULT ON)
    set(ARTK_BUILD_EXAMPLES_DEFAULT ON)
else()
    set(ARTK_IS_MAIN_PROJECT OFF)
    set(ARTK_INSTALL_DEFAULT OFF)
    set(ARTK_BUILD_EXAMPLES_DEFAULT OFF)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

option(ARTK_BUILD_EXAMPLES "Build sample programs and utilies" ${ARTK_BUILD_EXAMPLES_DEFAULT})
option(ARTK_USE_BOOST "Compile with boost (OpenCV calibration file support)" OFF)
option(ARTK_BUILD_DOCS "Build documentation (requires Doxygen)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ARTK_ENABLE_WARNINGS_AS_ERRORS "Enable warnings as errors" OFF)
option(ARTK_INSTALL "Install ARToolKitPlus library" ${ARTK_INSTALL_DEFAULT})
option(ARTK_BUILD_TESTS "Build unit tests" ${ARTK_IS_MAIN_PROJECT})
option(ARTK_BUILD_WITH_COVERAGE "Build with code coverage (only for Debug builds with GCC or Clang)" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()

# coverage is only supported for Debug and RelWithDebInfo builds with GCC or Clang
if(ARTK_BUILD_WITH_COVERAGE)
    if(NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU"))
        message(WARNING "Code coverage is only supported with GCC or Clang. Disabling ARTK_BUILD_WITH_COVERAGE.")
        set(ARTK_BUILD_WITH_COVERAGE OFF)
    elseif(NOT (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo"))
        message(WARNING "Code coverage is only supported for Debug or RelWithDebInfo builds. Disabling ARTK_BUILD_WITH_COVERAGE.")
        set(ARTK_BUILD_WITH_COVERAGE OFF)
    endif()
endif()

message(STATUS "ARToolKitPlus is main project: ${ARTK_IS_MAIN_PROJECT}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ARToolKitPlus version: ${PROJECT_VERSION}")
message(STATUS "Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "Build examples: ${ARTK_BUILD_EXAMPLES}")
message(STATUS "Treat warnings as errors: ${ARTK_ENABLE_WARNINGS_AS_ERRORS}")
message(STATUS "Install the library: ${ARTK_INSTALL}")
message(STATUS "Build documentation: ${ARTK_BUILD_DOCS}")
message(STATUS "Build tests: ${ARTK_BUILD_TESTS}")
message(STATUS "Build with code coverage: ${ARTK_BUILD_WITH_COVERAGE}")

# compile options for all targets depending on the compiler and the system
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(ARTK_COMPILE_OPTIONS "-Wall;-Wextra;-pedantic;-Wno-comment;-Wshadow;-Wnon-virtual-dtor;-Wpedantic;-Wconversion;-Wmisleading-indentation;-Wsign-conversion;-Wfloat-equal")
    if(ARTK_BUILD_WITH_COVERAGE)
        list(APPEND ARTK_COMPILE_OPTIONS "--coverage")
    endif()
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


set(ARTK_CXX_STANDARD 20)
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD ${ARTK_CXX_STANDARD})
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(CMAKE_CXX_STANDARD LESS ARTK_CXX_STANDARD)
  message(FATAL_ERROR "CMake CXX standard (${CMAKE_CXX_STANDARD}) is less than the required standard (${ARTK_CXX_STANDARD})")
endif()
set(ARTK_CXX_FEATURE cxx_std_${CMAKE_CXX_STANDARD})
message(STATUS "Using C++ standard: ${CMAKE_CXX_STANDARD}")


if(ARTK_USE_BOOST)
    add_definitions(-DAR_WITH_BOOST)
endif(ARTK_USE_BOOST)

set(AR_SOURCE_DIR ${PROJECT_SOURCE_DIR})

# Configure version header
set(ARTK_version_header_file "${CMAKE_CURRENT_BINARY_DIR}/include/ARToolKitPlus/version.h")
configure_file(
    ${AR_SOURCE_DIR}/cmake/version.h.in
    ${ARTK_version_header_file}
    @ONLY
)

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
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
  "$<INSTALL_INTERFACE:include/>")
target_compile_features(ARToolKitPlus PUBLIC ${ARTK_CXX_FEATURE})
if(ARTK_BUILD_WITH_COVERAGE)
    target_link_options(ARToolKitPlus PUBLIC --coverage)
endif()

set_target_properties(ARToolKitPlus PROPERTIES
  DEBUG_POSTFIX "d"
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION})

set(ARTK_namespace "${PROJECT_NAME}::")
add_library(${ARTK_namespace}ARToolKitPlus ALIAS ARToolKitPlus)

if(ARTK_BUILD_EXAMPLES)
    add_subdirectory(bin)
endif(ARTK_BUILD_EXAMPLES)

if(ARTK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(ARTK_BUILD_TESTS)

if(ARTK_BUILD_DOCS)
    add_subdirectory(doc)
endif(ARTK_BUILD_DOCS)



if (ARTK_INSTALL)
    include(GNUInstallDirs)

    ## Automatically generated files are stored here
    set(ARTK_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

    ####
    # Installation
    set(ARTK_config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
    set(ARTK_include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/")

    # Configuration
    set(ARTK_version_config "${ARTK_generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
    set(ARTK_project_config "${ARTK_generated_dir}/${PROJECT_NAME}Config.cmake")
    set(ARTK_targets_export_name "${PROJECT_NAME}Targets")

    # Include module with function 'write_basic_package_version_file'
    include(CMakePackageConfigHelpers)

    # Configure '<PROJECT-NAME>ConfigVersion.cmake'
    write_basic_package_version_file(
        "${ARTK_version_config}" VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion
    )

    # Configure '<PROJECT-NAME>Config.cmake'
    configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in"
        "${ARTK_project_config}"
        INSTALL_DESTINATION "${ARTK_config_install_dir}"
    )

    # Targets:
    install(
        TARGETS ARToolKitPlus
        EXPORT "${ARTK_targets_export_name}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${ARTK_include_install_dir}"
    )

    # Headers:
    install(
        DIRECTORY "${AR_SOURCE_DIR}/include/${PROJECT_NAME}"
        DESTINATION "${ARTK_include_install_dir}"
    )
    # install generated version header
    install(
        FILES "${ARTK_version_header_file}"
        DESTINATION "${ARTK_include_install_dir}/${PROJECT_NAME}")


    # Config
    install(
        FILES "${ARTK_project_config}" "${ARTK_version_config}"
        DESTINATION "${ARTK_config_install_dir}"
    )

    # Config
    install(
        EXPORT "${ARTK_targets_export_name}"
        NAMESPACE "${ARTK_namespace}"
        DESTINATION "${ARTK_config_install_dir}"
    )

    # if tools and examples are built, install the data files too as they are needed
    if(ARTK_BUILD_EXAMPLES)
        install(DIRECTORY "${AR_SOURCE_DIR}/sample/data" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
    endif(ARTK_BUILD_EXAMPLES)
endif()
