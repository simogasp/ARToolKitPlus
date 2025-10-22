set(ARTK_EXAMPLE_TARGETS "")

# examples
add_executable(simple ${AR_SOURCE_DIR}/sample/simple/main.cpp)
list(APPEND ARTK_EXAMPLE_TARGETS simple)

add_executable(multi ${AR_SOURCE_DIR}/sample/multi/main.cpp)
list(APPEND ARTK_EXAMPLE_TARGETS multi)

# tools
add_executable(patt2ppm ${AR_SOURCE_DIR}/tools/patt2ppm/main.cpp)
list(APPEND ARTK_EXAMPLE_TARGETS patt2ppm)

add_executable(ppm2patt ${AR_SOURCE_DIR}/tools/ppm2patt/main.cpp)
list(APPEND ARTK_EXAMPLE_TARGETS ppm2patt)

set(PAT_GEN_DIR ${AR_SOURCE_DIR}/tools/IdPatGen)
add_executable(idpatgen ${PAT_GEN_DIR}/main.cpp ${PAT_GEN_DIR}/PN/Image.cpp ${PAT_GEN_DIR}/PN/ImageTool.cpp)
list(APPEND ARTK_EXAMPLE_TARGETS idpatgen)

# test program
add_executable(test ${AR_SOURCE_DIR}/tools/test/main.cpp)
list(APPEND ARTK_EXAMPLE_TARGETS test)

foreach(target ${ARTK_EXAMPLE_TARGETS})
    target_compile_options(${target} PRIVATE ${ARTK_COMPILE_OPTIONS})
    target_compile_definitions(${target} PRIVATE ${ARTK_COMPILE_DEFINITIONS})
    target_compile_features(${target} PUBLIC ${ARTK_CXX_FEATURE})
    target_link_libraries(${target} ARToolKitPlus)
endforeach()

install(TARGETS ${ARTK_EXAMPLE_TARGETS}
  DESTINATION ${CMAKE_INSTALL_BINDIR})