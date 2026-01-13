# Copyright (C) 2026 Raven Computing

include(GenerateExportHeader)

set(RECKON_TARGET_LIB reckon CACHE STRING "Target name for the Reckon library")
set(RECKON_GENERATED_HEADERS ${CMAKE_BINARY_DIR}/headers)
if(RECKON_BUILD_SHARED_LIBS)
    set(RECKON_MAIN_LIB_TYPE SHARED)
else()
    set(RECKON_MAIN_LIB_TYPE STATIC)
endif()

#==================================[ TARGET ]==================================
# Reckon Library Objects

set(RECKON_TARGET_LIB_OBJ
    reckon_obj
    CACHE
    STRING
    "Target name for the Reckon objects library"
)

add_library(${RECKON_TARGET_LIB_OBJ} OBJECT)

target_sources(
    ${RECKON_TARGET_LIB_OBJ}
    PRIVATE
    "c/annotation.c"
    "c/characters.c"
    "c/debug.c"
    "c/encoding.c"
    "c/factories.c"
    "c/fileio.c"
    "$<$<PLATFORM_ID:Linux>:${CMAKE_CURRENT_SOURCE_DIR}/c/linux/fileio.c>"
    "$<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/c/win32/fileio.c>"
    "c/lang_c.c"
    "c/lang_java.c"
    "c/logical.c"
    "c/physical.c"
    "c/statistics.c"
    "c/tree.c"
    "c/words.c"
)

target_include_directories(
    ${RECKON_TARGET_LIB_OBJ}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${RECKON_GENERATED_HEADERS}>"
    PRIVATE
    c
)

target_link_libraries(
    ${RECKON_TARGET_LIB_OBJ}
    PUBLIC
    ${RECKON_DEPENDENCIES_LINK_TARGETS}
)

target_compile_definitions(
    ${RECKON_TARGET_LIB_OBJ}
    PRIVATE
    $<$<CONFIG:Debug>:RECKON_DEBUG=1>
    $<$<STREQUAL:${RECKON_MAIN_LIB_TYPE},SHARED>:${RECKON_TARGET_LIB}_EXPORTS>
)

set_target_properties(
    ${RECKON_TARGET_LIB_OBJ}
    PROPERTIES
    C_STANDARD ${RECKON_C_STANDARD}
    C_STANDARD_REQUIRED True
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN True
)

if("${RECKON_MAIN_LIB_TYPE}" STREQUAL "SHARED")
    set_target_properties(
        ${RECKON_TARGET_LIB_OBJ}
        PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif()

enable_compiler_warnings(
    ${RECKON_TARGET_LIB_OBJ}
    ${RECKON_IGNORE_WARNINGS}
)

if(RECKON_SOURCE_ANALYSIS)
    enable_source_compile_checks(
        ${RECKON_TARGET_LIB_OBJ}
    )
endif()

if(RECKON_USE_SANITIZERS)
    add_sanitizers(${RECKON_TARGET_LIB_OBJ})
endif()

#==================================[ TARGET ]==================================
# Exported Reckon Library

add_library(
    ${RECKON_TARGET_LIB}
    ${RECKON_MAIN_LIB_TYPE}
    $<TARGET_OBJECTS:${RECKON_TARGET_LIB_OBJ}>
)

target_include_directories(
    ${RECKON_TARGET_LIB}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${RECKON_GENERATED_HEADERS}>"
    PRIVATE
    c
)

target_link_libraries(
    ${RECKON_TARGET_LIB}
    PRIVATE
    ${RECKON_DEPENDENCIES_LINK_TARGETS}
)

set_target_properties(
    ${RECKON_TARGET_LIB}
    PROPERTIES
    C_STANDARD ${RECKON_C_STANDARD}
    C_STANDARD_REQUIRED True
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN True
)

set(RECKON_EXPORT_HEADER
    "${RECKON_GENERATED_HEADERS}/${RECKON_TARGET_LIB}_export.h"
)

generate_export_header(
    ${RECKON_TARGET_LIB}
    EXPORT_FILE_NAME
    "${RECKON_EXPORT_HEADER}"
)

#===================================[ TESTS ]==================================
# Library Reckon tests

if(RECKON_BUILD_TESTS)
    add_subdirectory(tests)
    if(RECKON_BUILD_TEST_COVERAGE)
        add_code_coverage(${RECKON_TARGET_LIB_OBJ})
        add_code_coverage(${RECKON_TARGET_LIB})
    endif()
endif()
