
find_package(GTest CONFIG QUIET)
if (NOT GTest_FOUND)
    message(STATUS "GTest not found. Fetching GoogleTest...")
    set(GTEST_VERSION_TAG "v1.17.0")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_TAG ${GTEST_VERSION_TAG}
        GIT_REPOSITORY https://github.com/google/googletest.git
        )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
message(STATUS "GoogleTest version: ${GTest_VERSION}")

set(ARTK_TEST_SOURCES
        unit/core/test_matrix.cpp
        unit/core/test_vector.cpp
        unit/test_version.cpp
)

# Discover and register tests
include(GoogleTest)

foreach(source_file ${ARTK_TEST_SOURCES})
    # get the filename without extension
    get_filename_component(test_name ${source_file} NAME_WE)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name}
        PRIVATE
            ARToolKitPlus
            GTest::gtest
            GTest::gtest_main
    )
    target_compile_features(${test_name} PUBLIC ${ARTK_CXX_FEATURE})
    target_compile_definitions(${test_name} PRIVATE ${ARTK_COMPILE_DEFINITIONS})
    target_compile_options(${test_name} PRIVATE ${ARTK_COMPILE_OPTIONS})

    gtest_discover_tests(${test_name})
endforeach()
