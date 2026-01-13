# Copyright (C) 2026 Raven Computing

# File operations are tested first because, for simplicity, even unit tests
# rely on the functions to read file content so that we can place multi-line
# input, expected text, golden samples etc. in text files, instead of defining
# them in the test code. For the sake of implementation simplicity, no separate
# abstractions for file reading are provided. Thus, when exercising unit tests
# the file I/O integration tests must have already proven that files can
# be read correctly as expected.

add_test_suite(
    TEST_SUITE_NAME        FileOpsIntegrationTest
    TEST_SUITE_TARGET      test_fileio
    TEST_SUITE_SOURCE      integration/c/test_fileio.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        CharacterCountUnitTest
    TEST_SUITE_TARGET      test_character_count
    TEST_SUITE_SOURCE      unit/c/test_character_count.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        WordCountUnitTest
    TEST_SUITE_TARGET      test_word_count
    TEST_SUITE_SOURCE      unit/c/test_word_count.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        PhysicalLinesUnitTest
    TEST_SUITE_TARGET      test_physical_lines
    TEST_SUITE_SOURCE      unit/c/test_physical_lines.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        LogicalLinesUnitTest
    TEST_SUITE_TARGET      test_logical_lines
    TEST_SUITE_SOURCE      unit/c/test_logical_lines.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        EncodingsUnitTest
    TEST_SUITE_TARGET      test_encodings
    TEST_SUITE_SOURCE      unit/c/test_encodings.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        FactoriesUnitTest
    TEST_SUITE_TARGET      test_factories
    TEST_SUITE_SOURCE      unit/c/test_factories.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        StatisticsCreationUnitTest
    TEST_SUITE_TARGET      test_statistics_creation
    TEST_SUITE_SOURCE      unit/c/test_statistics_creation.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        StatisticsUnitTest
    TEST_SUITE_TARGET      test_statistics
    TEST_SUITE_SOURCE      unit/c/test_statistics.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        CountOptionsUnitTest
    TEST_SUITE_TARGET      test_count_options
    TEST_SUITE_SOURCE      unit/c/test_count_options.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        CLanguageUnitTest
    TEST_SUITE_TARGET      test_lang_c
    TEST_SUITE_SOURCE      unit/c/test_lang_c.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_test_suite(
    TEST_SUITE_NAME        JavaLanguageUnitTest
    TEST_SUITE_TARGET      test_lang_java
    TEST_SUITE_SOURCE      unit/c/test_lang_java.c
    TEST_SUITE_LINK        ${RECKON_TARGET_LIB_OBJ}
)

add_compile_definitions(
    RECKON_TEST_PATH_RES_BASE="${CMAKE_CURRENT_SOURCE_DIR}/res"
)
