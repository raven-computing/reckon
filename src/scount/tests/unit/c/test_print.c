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
#include <string.h>

#include "unity.h"

#include "reckon/reckon.h"
#include "scount.h"

void setUp(void) { }

void tearDown(void) { }

static RcnCountStatistics* mkStats(
    char* fileName,
    size_t size,
    RcnCount logical,
    RcnCount physical,
    RcnCount words,
    RcnCount characters,
    RcnCount sourceSize
) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    stats->count.size = size;
    stats->count.files = calloc(size, sizeof(RcnSourceFile));
    stats->count.results = calloc(size, sizeof(RcnCountResultGroup));
    for (size_t i = 0; i < size; ++i) {
        char* name = fileName ? strdup(fileName) : NULL;
        stats->count.files[i].name = name;
        stats->count.files[i].path = name;
        stats->count.files[i].extension = name;
        stats->count.results[i].logicalLines = logical;
        stats->count.results[i].physicalLines = physical;
        stats->count.results[i].words = words;
        stats->count.results[i].characters = characters;
        stats->count.results[i].sourceSize = sourceSize;
    }
    stats->logicalLines[RCN_LANG_JAVA] = logical;
    stats->physicalLines[RCN_LANG_JAVA] = physical;
    stats->words[RCN_LANG_JAVA] = words;
    stats->characters[RCN_LANG_JAVA] = characters;
    stats->sourceSize[RCN_LANG_JAVA] = sourceSize;
    stats->totalLogicalLines = logical * size;
    stats->totalPhysicalLines = physical * size;
    stats->totalWords = words * size;
    stats->totalCharacters = characters * size;
    stats->totalSourceSize = sourceSize * size;
    return stats;
}

// NOLINTBEGIN(readability-magic-numbers)

void testPrintSingleResultBasic(void) {
    char* expected = (
        "File: SomeFile.java\n\n"
        "  Logical Lines of Code (LLC):        123\n"
        "  Physical Lines        (PHL):        456\n"
        "  Words                 (WRD):       7890\n"
        "  Characters            (CHR):       8765\n"
        "  Source Size in Bytes  (SZE):       4321\n\n"
    );
    RcnCountStatistics* stats = mkStats(
        "SomeFile.java", 1, 123, 456, 7890, 8765, 4321
    );
    PrintBuffer buffer = printResultSingle(stats);

    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_TRUE(buffer.size > 0);
    TEST_ASSERT_EQUAL_STRING(expected, buffer.text);
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

void testPrintSingleResultWithUnknownFileName(void) {
    char* expected = (
        "File: (unknown)\n\n"
        "  Logical Lines of Code (LLC):          1\n"
        "  Physical Lines        (PHL):          2\n"
        "  Words                 (WRD):          3\n"
        "  Characters            (CHR):          4\n"
        "  Source Size in Bytes  (SZE):          5\n\n"
    );
    RcnCountStatistics* stats = mkStats(NULL, 1, 1, 2, 3, 4, 5);
    PrintBuffer buffer = printResultSingle(stats);
    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_TRUE(buffer.size > 0);
    TEST_ASSERT_EQUAL_STRING(expected, buffer.text);
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

void testPrintMultiResultBasic(void) {
    char* expected = (
        "Directory: myDirectory\n"
        "Scanned files: 3\n"
        "\n"
        "  o---------- File ----------o--- LLC ---o--- PHL ---o--- WRD ---o--- CHR ---o--- SZE ---o\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  o--------------------------o-----------o-----------o-----------o-----------o-----------o\n"
        "\n"
        "Summary:\n"
        "\n"
        "  o-------- Language --------o--- LLC ---o--- PHL ---o--- WRD ---o--- CHR ---o--- SZE ---o\n"
        "  | Java                     |     1     |     2     |     3     |     4     |     5     |\n"
        "  o==========================o===========o===========o===========o===========o===========o\n"
        "  | Total:                   |     3     |     6     |     9     |    12     |    15     |\n"
        "  o==========================o===========o===========o===========o===========o===========o\n"
        "\n"
        "\n"
    );
    RcnCountStatistics* stats = mkStats(
        "SomeFile.java",
        3, 1, 2, 3, 4, 5
    );
    PrintBuffer buffer = printResultsMultiple(
        "/some/path/to/myDirectory",
        stats
    );
    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_EQUAL_INT(strlen(expected), buffer.size);
    TEST_ASSERT_EQUAL_STRING(expected, buffer.text);
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

void testPrintMultiResultForDirectoryInputWithManyFiles(void) {
    char* expected = (
        "Directory: myDirectory\n"
        "Scanned files: 18\n"
        "\n"
        "  o---------- File ----------o--- LLC ---o--- PHL ---o--- WRD ---o--- CHR ---o--- SZE ---o\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  |            ..            |    ...    |    ...    |    ...    |    ...    |    ...    |\n"
        "  | SomeFile.java            |     1     |     2     |     3     |     4     |     5     |\n"
        "  o--------------------------o-----------o-----------o-----------o-----------o-----------o\n"
        "\n"
        "Summary:\n"
        "\n"
        "  o-------- Language --------o--- LLC ---o--- PHL ---o--- WRD ---o--- CHR ---o--- SZE ---o\n"
        "  | Java                     |     1     |     2     |     3     |     4     |     5     |\n"
        "  o==========================o===========o===========o===========o===========o===========o\n"
        "  | Total:                   |    18     |    36     |    54     |    72     |    90     |\n"
        "  o==========================o===========o===========o===========o===========o===========o\n"
        "\n"
        "\n"
    );
    RcnCountStatistics* stats = mkStats(
        "SomeFile.java",
        18, 1, 2, 3, 4, 5
    );
    PrintBuffer buffer = printResultsMultiple(
        "/some/path/to/myDirectory",
        stats
    );
    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_EQUAL_INT(strlen(expected), buffer.size);
    TEST_ASSERT_EQUAL_STRING(expected, buffer.text);
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

void testPrintMultiResultWithLongFileNames(void) {
    char* longFileName = (
        "SourceFileWithSomeLongNameSuchThatItHasToBeTruncatedWhenPrinted.java"
    );
    RcnCountStatistics* stats = mkStats(
        longFileName,
        3, 1, 2, 3, 4, 5
    );
    PrintBuffer buffer = printResultsMultiple(
        "/some/path/to/myDirectory",
        stats
    );
    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_TRUE(buffer.size > 0);
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "...catedWhenPrinted.java"));
    TEST_ASSERT_NULL(strstr(buffer.text, longFileName));
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

void testPrintMultiResultWithErrorInResultGroup(void) {
    RcnCountStatistics* stats = mkStats(
        "SomeFile0.java",
        3, 1, 2, 3, 4, 5
    );
    stats->count.files[1].path[8] = '1';
    stats->count.files[2].path[8] = '2';
    stats->count.results[1].state.errorCode = RCN_ERR_SYNTAX_ERROR;
    PrintBuffer buffer = printResultsMultiple(
        "/some/path/to/myDirectory",
        stats
    );
    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_TRUE(buffer.size > 0);
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "Scanned files: 3"));
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "SomeFile0.java"));
    TEST_ASSERT_NULL(strstr(buffer.text, "SomeFile1.java"));
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "SomeFile2.java"));
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

void testPrintMultiResultWithBigNumbers(void) {
    RcnCount big = 12345678901234567;
    RcnCountStatistics* stats = mkStats(
        "SomeFile0.java",
        2, big, big, big, big, big
    );
    stats->count.files[1].path[8] = '1';
    PrintBuffer buffer = printResultsMultiple(
        "/some/path/to/myDirectory",
        stats
    );
    TEST_ASSERT_NOT_NULL(buffer.text);
    TEST_ASSERT_TRUE(buffer.size > 0);
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "Scanned files: 2"));
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "SomeFile0.java"));
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "SomeFile1.java"));
    TEST_ASSERT_NOT_NULL(strstr(buffer.text, "| 123456789 |"));
    TEST_ASSERT_NULL(strstr(buffer.text, "| 12345678901234567 |"));
    free(buffer.text);
    rcnFreeCountStatistics(stats);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testPrintSingleResultBasic);
    RUN_TEST(testPrintSingleResultWithUnknownFileName);
    RUN_TEST(testPrintMultiResultBasic);
    RUN_TEST(testPrintMultiResultForDirectoryInputWithManyFiles);
    RUN_TEST(testPrintMultiResultWithLongFileNames);
    RUN_TEST(testPrintMultiResultWithErrorInResultGroup);
    RUN_TEST(testPrintMultiResultWithBigNumbers);
    return UNITY_END();
}
