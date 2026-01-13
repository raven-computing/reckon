# Copyright (C) 2026 Raven Computing

add_test_suite(
    TEST_SUITE_NAME        ParseArgsUnitTest
    TEST_SUITE_TARGET      test_parse_args
    TEST_SUITE_SOURCE      unit/c/test_parse_args.c
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION_CORE}
)

add_test_suite(
    TEST_SUITE_NAME        LoggingUnitTest
    TEST_SUITE_TARGET      test_logging
    TEST_SUITE_SOURCE      unit/c/test_logging.c
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION_CORE}
)

add_test_suite(
    TEST_SUITE_NAME        PrintUnitTest
    TEST_SUITE_TARGET      test_print
    TEST_SUITE_SOURCE      unit/c/test_print.c
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION_CORE}
)

add_test_suite(
    TEST_SUITE_NAME        StatisticsInputUnitTest
    TEST_SUITE_TARGET      test_statistics_input
    TEST_SUITE_SOURCE      unit/c/test_statistics_input.c
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION_CORE}
)

add_test_suite(
    TEST_SUITE_NAME        ScountFunctionalityTest
    TEST_SUITE_TARGET      test_scount
    TEST_SUITE_SOURCE      functionality/sh/test_scount.sh
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION}
)

add_test_suite(
    TEST_SUITE_NAME        ScountInvalidInputFunctionalityTest
    TEST_SUITE_TARGET      test_scount_invalid_input
    TEST_SUITE_SOURCE      functionality/sh/test_scount_invalid_input.sh
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION}
)

add_test_suite(
    TEST_SUITE_NAME        ScountMiscFunctionalityTest
    TEST_SUITE_TARGET      test_scount_misc
    TEST_SUITE_SOURCE      functionality/sh/test_scount_misc.sh
    TEST_SUITE_LINK        ${RECKON_TARGET_APPLICATION}
)

if(WIN32)
    # LoggingUnitTest is currently not compatible with Windows
    # due to unavailable open_memstream(). Must be rewritten,
    # see comment in test_logging.c. For now, skip it on Windows.
    set_tests_properties(
        LoggingUnitTest
        PROPERTIES
        SKIP_REGULAR_EXPRESSION "Test case LoggingUnitTest skipped on Windows"
    )
endif()
