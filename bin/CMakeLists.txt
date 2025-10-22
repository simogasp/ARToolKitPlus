set(ARTK_TOOLS_TARGETS "")
set(ARTK_TEST_REGRESSION_TARGETS "")

# examples
add_executable(simple ${AR_SOURCE_DIR}/sample/simple/main.cpp)
target_include_directories(simple PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND ARTK_TEST_REGRESSION_TARGETS simple)

add_executable(multi ${AR_SOURCE_DIR}/sample/multi/main.cpp)
target_include_directories(multi PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND ARTK_TEST_REGRESSION_TARGETS multi)

# tools
add_executable(patt2ppm ${AR_SOURCE_DIR}/tools/patt2ppm/main.cpp)
list(APPEND ARTK_TOOLS_TARGETS patt2ppm)

add_executable(ppm2patt ${AR_SOURCE_DIR}/tools/ppm2patt/main.cpp)
list(APPEND ARTK_TOOLS_TARGETS ppm2patt)

set(PAT_GEN_DIR ${AR_SOURCE_DIR}/tools/IdPatGen)
add_executable(idpatgen ${PAT_GEN_DIR}/main.cpp ${PAT_GEN_DIR}/PN/Image.cpp ${PAT_GEN_DIR}/PN/ImageTool.cpp)
list(APPEND ARTK_TOOLS_TARGETS idpatgen)

# test program
# Configure test data paths
set(TEST_DATA_DIR "${AR_SOURCE_DIR}/sample/data")
configure_file(
    ${AR_SOURCE_DIR}/tools/test/test_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.h
    @ONLY
)
add_executable(test_detection ${AR_SOURCE_DIR}/tools/test/main.cpp)
target_include_directories(test_detection PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND ARTK_TEST_REGRESSION_TARGETS test_detection)

add_executable(test_version ${AR_SOURCE_DIR}/tools/test/test_version.cpp)
list(APPEND ARTK_TEST_REGRESSION_TARGETS test_version)

foreach(target ${ARTK_TEST_REGRESSION_TARGETS} ${ARTK_TOOLS_TARGETS})
    target_compile_options(${target} PRIVATE ${ARTK_COMPILE_OPTIONS})
    target_compile_definitions(${target} PRIVATE ${ARTK_COMPILE_DEFINITIONS})
    target_compile_features(${target} PUBLIC ${ARTK_CXX_FEATURE})
    target_link_libraries(${target} ARToolKitPlus)
endforeach()

install(TARGETS ${ARTK_TOOLS_TARGETS}
  DESTINATION ${CMAKE_INSTALL_BINDIR})
