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

#include "unity.h"

#include "reckon/reckon.h"
#include "fileio.h"

void setUp(void) { }

void tearDown(void) { }

// NOLINTBEGIN(readability-magic-numbers)

void testCountWithVariousNullInputs(void) {
    RcnStatOptions options = {0};
    rcnCount(NULL, options); // Must be handled gracefully

    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    rcnCount(stats, options);
    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "No input files provided",
        stats->state.errorMessage
    );

    stats->count.files = calloc(1, sizeof(RcnSourceFile));
    rcnCount(stats, options);
    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_STRING(
        "No result list provided",
        stats->state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);

    stats->count.results = calloc(1, sizeof(RcnCountResultGroup));
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(0, stats->totalSourceSize);
    rcnFreeCountStatistics(stats);
}

void testCountWithFileThatHasIOErrorSetAfterRead(void) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    RcnSourceFile* file = newSourceFile("Dummy.java");
    file->isContentRead = true;
    file->status = RCN_FILE_OP_IO_ERROR;
    RcnCountResultGroup* result = calloc(1, sizeof(RcnCountResultGroup));
    stats->count.size = 1;
    stats->count.files = file;
    stats->count.results = result;
    RcnStatOptions options = {0};
    options.stopOnError = true;

    rcnCount(stats, options);

    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_FALSE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result->state.errorCode);
    rcnFreeCountStatistics(stats);
}

void testCountWithFileWhenStatusIsOkButContentIsNull(void) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    RcnSourceFile* file = newSourceFile("Dummy.java");
    file->isContentRead = true;
    file->content = (RcnSourceText){
        .text = NULL,
        .size = 42
    };
    file->status = RCN_FILE_OP_OK;
    RcnCountResultGroup* result = calloc(1, sizeof(RcnCountResultGroup));
    stats->count.size = 1;
    stats->count.files = file;
    stats->count.results = result;
    RcnStatOptions options = {0};
    options.stopOnError = true;

    rcnCount(stats, options);

    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_FALSE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result->state.errorCode);
    rcnFreeCountStatistics(stats);
}

void testCountWithFileWhenContentIsNullAndStatusIsFileError(void) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    RcnSourceFile* file = newSourceFile("Dummy.java");
    file->status = RCN_FILE_OP_FILE_TOO_LARGE;
    RcnCountResultGroup* result = calloc(1, sizeof(RcnCountResultGroup));
    stats->count.size = 1;
    stats->count.files = file;
    stats->count.results = result;
    RcnStatOptions options = {0};

    rcnCount(stats, options);

    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Failed to read file content",
        stats->state.errorMessage
    );
    TEST_ASSERT_FALSE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result->state.errorCode);
        TEST_ASSERT_EQUAL_STRING(
        "Failed to read file content",
        result->state.errorMessage
    );
    rcnFreeCountStatistics(stats);
}

void testCountWhenFileHasUnsupportedFormat(void) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    // We will likely never support COBOL, what a shame :(
    RcnSourceFile* file = newSourceFile("old.cob");
    RcnCountResultGroup* result = calloc(1, sizeof(RcnCountResultGroup));
    stats->count.size = 1;
    stats->count.files = file;
    stats->count.results = result;
    RcnStatOptions options = {0};

    rcnCount(stats, options);

    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_UNSUPPORTED_FORMAT, stats->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "The source format is not supported",
        stats->state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file->status);
    TEST_ASSERT_FALSE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_UNSUPPORTED_FORMAT, result->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "The source format is not supported",
        result->state.errorMessage
    );
    rcnFreeCountStatistics(stats);
}

void testCountWithMultipleFilesWhenOneFileHasError(void) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    RcnSourceFile* files = calloc(2, sizeof(RcnSourceFile));
    RcnSourceFile* file1 = &files[0];
    RcnSourceFile* file2 = &files[1];
    initSourceFile(file1, "file_with_error.c");
    file1->status = RCN_FILE_OP_FILE_TOO_LARGE;
    initSourceFile(file2, "normal_file.c");
    file2->content = (RcnSourceText){
        .text = "int main() { return 0; }",
        .size = 24
    };
    file2->isContentRead = true;
    RcnCountResultGroup* results = calloc(2, sizeof(RcnCountResultGroup));
    RcnCountResultGroup* result1 = &results[0];
    RcnCountResultGroup* result2 = &results[1];
    stats->count.size = 2;
    stats->count.files = files;
    stats->count.results = results;
    RcnStatOptions options = {
        .keepFileContent = true
    };

    rcnCount(stats, options);

    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Failed to read file content",
        stats->state.errorMessage
    );
    TEST_ASSERT_FALSE(result1->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result1->state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Failed to read file content",
        result1->state.errorMessage
    );
    TEST_ASSERT_FALSE(result1->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result1->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result1->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result1->words);
    TEST_ASSERT_EQUAL_INT(0, result1->characters);
    TEST_ASSERT_EQUAL_INT(0, result1->sourceSize);
    TEST_ASSERT_TRUE(result2->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result2->state.errorCode);
    TEST_ASSERT_NULL(result2->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(2, result2->logicalLines);
    TEST_ASSERT_EQUAL_INT(1, result2->physicalLines);
    TEST_ASSERT_EQUAL_INT(6, result2->words);
    TEST_ASSERT_EQUAL_INT(24, result2->characters);
    TEST_ASSERT_EQUAL_INT(24, result2->sourceSize);
    TEST_ASSERT_TRUE(result2->isProcessed);
    file2->content = (RcnSourceText){0};
    rcnFreeCountStatistics(stats);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCountWithVariousNullInputs);
    RUN_TEST(testCountWithFileThatHasIOErrorSetAfterRead);
    RUN_TEST(testCountWithFileWhenStatusIsOkButContentIsNull);
    RUN_TEST(testCountWithFileWhenContentIsNullAndStatusIsFileError);
    RUN_TEST(testCountWhenFileHasUnsupportedFormat);
    RUN_TEST(testCountWithMultipleFilesWhenOneFileHasError);
    return UNITY_END();
}
