disable_compiler_warnings(unity)
target_compile_definitions(unity PUBLIC UNITY_INCLUDE_CONFIG_H)
target_include_directories(
    unity
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/lib/tests/include>"
)
