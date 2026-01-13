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
#include <string.h>

#include "unity.h"

#include "reckon/reckon.h"

void setUp(void) { }

void tearDown(void) { }

void assertZeroInitializedStats(RcnCountStatistics* stats) {
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(0, stats->totalSourceSize);
    TEST_ASSERT_EACH_EQUAL_UINT64(
        0, stats->logicalLines, RECKON_NUM_SUPPORTED_FORMATS
    );
    TEST_ASSERT_EACH_EQUAL_UINT64(
        0, stats->physicalLines, RECKON_NUM_SUPPORTED_FORMATS
    );
    TEST_ASSERT_EACH_EQUAL_UINT64(
        0, stats->words, RECKON_NUM_SUPPORTED_FORMATS
    );
    TEST_ASSERT_EACH_EQUAL_UINT64(
        0, stats->characters, RECKON_NUM_SUPPORTED_FORMATS
    );
    TEST_ASSERT_EACH_EQUAL_UINT64(
        0, stats->sourceSize, RECKON_NUM_SUPPORTED_FORMATS
    );
}

void assertZeroInitializedStatsOk(RcnCountStatistics* stats) {
    assertZeroInitializedStats(stats);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_FALSE(stats->state.ok);
}

void assertZeroInitializedStatsWithError(
    RcnCountStatistics* stats,
    RcnErrorCode expectedError,
    char* expectedErrorMessage
) {
    assertZeroInitializedStats(stats);
    TEST_ASSERT_EQUAL_INT(expectedError, stats->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(expectedErrorMessage, stats->state.errorMessage);
    TEST_ASSERT_FALSE(stats->state.ok);
}

void assertUnreadFile(
    RcnSourceFile* file,
    char* path,
    char* name,
    char* extension
) {
    TEST_ASSERT_EQUAL_STRING(path, file->path);
    TEST_ASSERT_EQUAL_STRING(name, file->name);
    TEST_ASSERT_EQUAL_STRING(extension, file->extension);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file->content.size);
    TEST_ASSERT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file->status);
}

void assertZeroInitializedResult(RcnCountResultGroup* result) {
    TEST_ASSERT_EQUAL_INT(0, result->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->words);
    TEST_ASSERT_EQUAL_INT(0, result->characters);
    TEST_ASSERT_EQUAL_INT(0, result->sourceSize);
    TEST_ASSERT_FALSE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result->state.errorCode);
    TEST_ASSERT_NULL(result->state.errorMessage);
    TEST_ASSERT_FALSE(result->isProcessed);
}

void testCreateStatisticsWithNullPathReturnsNull(void) {
    RcnCountStatistics* stats = rcnCreateCountStatistics(NULL);
    TEST_ASSERT_NULL(stats);
}

void testCreateStatisticsWithPathToRegularFile(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/txt/1sample1.txt";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    TEST_ASSERT_NOT_NULL(stats);
    assertZeroInitializedStatsOk(stats);
    TEST_ASSERT_EQUAL_INT(1, stats->count.size);
    TEST_ASSERT_EQUAL_INT(0, stats->count.sizeProcessed);
    TEST_ASSERT_NOT_NULL(stats->count.files);
    TEST_ASSERT_NOT_NULL(stats->count.results);
    assertUnreadFile(&stats->count.files[0], path, "1sample1.txt", "txt");
    assertZeroInitializedResult(&stats->count.results[0]);
    rcnFreeCountStatistics(stats);
}

void testCreateStatisticsWithPathToDirectory(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/txt/res2";
    char* pathFile1 = RECKON_TEST_PATH_RES_BASE "/txt/res2/2sample1.txt";
    char* pathFile2 = RECKON_TEST_PATH_RES_BASE "/txt/res2/2sample2.txt";
    char* pathFile3 = RECKON_TEST_PATH_RES_BASE "/txt/res2/res3/3sample1.txt";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    TEST_ASSERT_NOT_NULL(stats);
    assertZeroInitializedStatsOk(stats);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(0, stats->count.sizeProcessed);
    TEST_ASSERT_NOT_NULL(stats->count.files);
    TEST_ASSERT_NOT_NULL(stats->count.results);
    assertUnreadFile(&stats->count.files[0], pathFile1, "2sample1.txt", "txt");
    assertUnreadFile(&stats->count.files[1], pathFile2, "2sample2.txt", "txt");
    assertUnreadFile(&stats->count.files[2], pathFile3, "3sample1.txt", "txt");
    assertZeroInitializedResult(&stats->count.results[0]);
    assertZeroInitializedResult(&stats->count.results[1]);
    assertZeroInitializedResult(&stats->count.results[2]);
    rcnFreeCountStatistics(stats);
}

void testCreateStatisticsWithPathToNonexistingFile(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/this-does-not-exist";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    TEST_ASSERT_NOT_NULL(stats);
    assertZeroInitializedStatsWithError(
        stats, RCN_ERR_INVALID_INPUT, "No such file or directory"
    );
    TEST_ASSERT_EQUAL_INT(0, stats->count.size);
    TEST_ASSERT_EQUAL_INT(0, stats->count.sizeProcessed);
    TEST_ASSERT_NULL(stats->count.files);
    TEST_ASSERT_NULL(stats->count.results);
    rcnFreeCountStatistics(stats);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCreateStatisticsWithNullPathReturnsNull);
    RUN_TEST(testCreateStatisticsWithPathToRegularFile);
    RUN_TEST(testCreateStatisticsWithPathToDirectory);
    RUN_TEST(testCreateStatisticsWithPathToNonexistingFile);
    return UNITY_END();
}
