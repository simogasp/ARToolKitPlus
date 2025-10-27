cmake_minimum_required(VERSION 3.20)

project(test_ATKP_as3rdparty LANGUAGES CXX
        DESCRIPTION "Test ARToolKitPlus as 3rd party library either as installed library or using fetchcontent")

find_package(ARToolKitPlus QUIET)
if(NOT ARToolKitPlus_FOUND)
    message(STATUS "ARToolKitPlus not found, fetching from github...")
    include(FetchContent)
    FetchContent_Declare(
        ARToolKitPlus
        GIT_REPOSITORY https://github.com/simogasp/ARToolKitPlus.git
        GIT_TAG        develop
    )
    FetchContent_MakeAvailable(ARToolKitPlus)

endif()
message(STATUS "Found ARToolKitPlus, version ${ARToolKitPlus_VERSION}")
add_executable(test_ATKP_as3rdparty main.cpp)
target_link_libraries(test_ATKP_as3rdparty PUBLIC ARToolKitPlus::ARToolKitPlus)