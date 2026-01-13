/*
 * Copyright (C) 2026 Raven Computing
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"

#include "scount.h"

static char* LOG_OUT_BUFFER = NULL;
static size_t LOG_OUT_BUFFER_SIZE = 0;
static char* LOG_ERR_BUFFER = NULL;
static size_t LOG_ERR_BUFFER_SIZE = 0;

#ifndef _WIN32
void setUp(void) {
    LOG_LEVEL = LOG_LEVEL_VERBOSE;
    // open_memstream() is specified by POSIX but Windows doesn't implement it.
    // (of course!!!)
    // On Windows maybe use CreateStreamOnHGlobal(),
    // or just create a FILE* with tmpfile() as a cross-platform alternative.
    LOG_STREAM_OUT = open_memstream(&LOG_OUT_BUFFER, &LOG_OUT_BUFFER_SIZE);
    LOG_STREAM_ERR = open_memstream(&LOG_ERR_BUFFER, &LOG_ERR_BUFFER_SIZE);

}

void tearDown(void) {
    LOG_LEVEL = LOG_LEVEL_DISABLED;
    (void) fclose(LOG_STREAM_OUT);
    (void) fclose(LOG_STREAM_ERR);
    free(LOG_OUT_BUFFER);
    LOG_OUT_BUFFER = NULL;
    LOG_OUT_BUFFER_SIZE = 0;
    free(LOG_ERR_BUFFER);
    LOG_ERR_BUFFER = NULL;
    LOG_ERR_BUFFER_SIZE = 0;
}
#else
void setUp(void) { /* NOOP on Windows */ }
void tearDown(void) { /* NOOP on Windows */ }
#endif

// NOLINTBEGIN(readability-magic-numbers)

void testErrorLogging(void) {
    LOG_LEVEL = LOG_LEVEL_ERROR;
    char* message = "This is an error message that should be logged.";
    logE("%s", message);
    TEST_ASSERT_EQUAL_INT(0, LOG_OUT_BUFFER_SIZE);
    size_t expectedSize = strlen(message) + 1; // NL char
    TEST_ASSERT_EQUAL_INT(expectedSize, LOG_ERR_BUFFER_SIZE);
    TEST_ASSERT_NOT_NULL(strstr(LOG_ERR_BUFFER, message));
}

void testLogErrorLevelWhenDisabled(void) {
    LOG_LEVEL = LOG_LEVEL_DISABLED;
    logE("This is an error message that should not appear.");
    TEST_ASSERT_EQUAL_INT(0, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
}

void testWarningLogging(void) {
    LOG_LEVEL = LOG_LEVEL_WARNING;
    char* message = "This is a warning that should be logged.";
    logW("%s", message);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
    size_t expectedSize = strlen(message) + 10; // 'Warning: ' + NL char
    TEST_ASSERT_EQUAL_INT(expectedSize, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_NOT_NULL(strstr(LOG_OUT_BUFFER, message));
}

void testLogWarningLevelWhenDisabled(void) {
    LOG_LEVEL = LOG_LEVEL_DISABLED;
    logW("This is a warning message that should not appear.");
    TEST_ASSERT_EQUAL_INT(0, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
}

void testInfoLogging(void) {
    LOG_LEVEL = LOG_LEVEL_INFO;
    char* message = "This is an info message that should be logged.";
    logI("%s", message);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
    size_t expectedSize = strlen(message) + 1; // NL char
    TEST_ASSERT_EQUAL_INT(expectedSize, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_NOT_NULL(strstr(LOG_OUT_BUFFER, message));
}

void testLogInfoLevelWhenDisabled(void) {
    LOG_LEVEL = LOG_LEVEL_DISABLED;
    logI("This is an info message that should not appear.");
    TEST_ASSERT_EQUAL_INT(0, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
}

void testVerboseLogging(void) {
    LOG_LEVEL = LOG_LEVEL_VERBOSE;
    char* message = "This is a verbose message that should be logged.";
    logV("%s", message);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
    size_t expectedSize = strlen(message) + 1; // NL char
    TEST_ASSERT_EQUAL_INT(expectedSize, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_NOT_NULL(strstr(LOG_OUT_BUFFER, message));
}

void testLogVerboseLevelWhenDisabled(void) {
    LOG_LEVEL = LOG_LEVEL_DISABLED;
    logV("This is a verbose message that should not appear.");
    TEST_ASSERT_EQUAL_INT(0, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
}

void testLogStdoutWhenDisabled(void) {
    LOG_LEVEL = LOG_LEVEL_DISABLED;
    logStdout("This is text logged on stdout that should not appear.");
    TEST_ASSERT_EQUAL_INT(0, LOG_OUT_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_INT(0, LOG_ERR_BUFFER_SIZE);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
#ifndef _WIN32
    UNITY_BEGIN();
    RUN_TEST(testErrorLogging);
    RUN_TEST(testLogErrorLevelWhenDisabled);
    RUN_TEST(testWarningLogging);
    RUN_TEST(testLogWarningLevelWhenDisabled);
    RUN_TEST(testInfoLogging);
    RUN_TEST(testLogInfoLevelWhenDisabled);
    RUN_TEST(testVerboseLogging);
    RUN_TEST(testLogVerboseLevelWhenDisabled);
    RUN_TEST(testLogStdoutWhenDisabled);
    return UNITY_END();
#else
    printf("Test case LoggingUnitTest skipped on Windows");
    return 127;
#endif
}
