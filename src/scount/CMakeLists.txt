# Copyright (C) 2026 Raven Computing

#==================================[ TARGET ]==================================
# Application scount core libraries

set(RECKON_TARGET_APPLICATION_CORE scountcore)

add_library(
    ${RECKON_TARGET_APPLICATION_CORE}
    STATIC
    c/annotation.c
    c/arguments.c
    c/logging.c
    c/print.c
    c/statistics.c
)

target_include_directories(
    ${RECKON_TARGET_APPLICATION_CORE}
    PRIVATE
    c
)

target_link_libraries(
    ${RECKON_TARGET_APPLICATION_CORE}
    PUBLIC
    ${RECKON_TARGET_LIB}
)

target_compile_definitions(
    ${RECKON_TARGET_APPLICATION_CORE}
    PRIVATE
    RECKON_VERSION="${RECKON_VERSION}"
)

set_target_properties(
    ${RECKON_TARGET_APPLICATION_CORE}
    PROPERTIES
    C_STANDARD ${RECKON_C_STANDARD}
    C_STANDARD_REQUIRED True
)

enable_compiler_warnings(
    ${RECKON_TARGET_APPLICATION_CORE}
    ${RECKON_IGNORE_WARNINGS}
)

if(RECKON_SOURCE_ANALYSIS)
    enable_source_compile_checks(
        ${RECKON_TARGET_APPLICATION_CORE}
    )
endif()

if(RECKON_USE_SANITIZERS)
    add_sanitizers(${RECKON_TARGET_APPLICATION_CORE})
endif()

#==================================[ TARGET ]==================================
# Application scount main executable

set(RECKON_TARGET_APPLICATION scount)

add_executable(
    ${RECKON_TARGET_APPLICATION}
    c/main.c
)

target_include_directories(
    ${RECKON_TARGET_APPLICATION}
    PRIVATE
    c
)

target_link_libraries(
    ${RECKON_TARGET_APPLICATION}
    PRIVATE
    ${RECKON_TARGET_APPLICATION_CORE}
)

set_target_properties(
    ${RECKON_TARGET_APPLICATION}
    PROPERTIES
    C_STANDARD ${RECKON_C_STANDARD}
    C_STANDARD_REQUIRED True
)

set_stripped_executable(${RECKON_TARGET_APPLICATION})

enable_compiler_warnings(
    ${RECKON_TARGET_APPLICATION}
    ${RECKON_IGNORE_WARNINGS}
)

if(RECKON_SOURCE_ANALYSIS)
    enable_source_compile_checks(
        ${RECKON_TARGET_APPLICATION}
    )
endif()

#===================================[ TESTS ]==================================
# Application scount tests

if(RECKON_BUILD_TESTS)
    add_subdirectory(tests)
    if(RECKON_BUILD_TEST_COVERAGE)
        add_code_coverage(${RECKON_TARGET_APPLICATION_CORE})
        add_code_coverage(${RECKON_TARGET_APPLICATION})
    endif()
endif()
