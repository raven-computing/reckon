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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "unity.h"

#include "reckon/reckon.h"
#include "fileio.h"

void setUp(void) { }

void tearDown(void) { }

// NOLINTBEGIN(readability-magic-numbers)

void testCountStatisticsOnlyLogicalLines(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java/Sample.java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .operations = RCN_OPT_COUNT_LOGICAL_LINES,
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(104, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(4709, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(104, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(4709, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(1, stats->count.size);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, stats->count.files[0].status);
    RcnCountResultGroup* result = &stats->count.results[0];
    TEST_ASSERT_TRUE(result->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->words);
    TEST_ASSERT_EQUAL_INT(0, result->characters);
    TEST_ASSERT_EQUAL_INT(4709, result->sourceSize);
    TEST_ASSERT_TRUE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result->state.errorCode);
    TEST_ASSERT_NULL(result->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsOnlyPhysicalLines(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java/Sample.java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .operations = RCN_OPT_COUNT_PHYSICAL_LINES,
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(188, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(4709, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(188, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(4709, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(1, stats->count.size);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, stats->count.files[0].status);
    RcnCountResultGroup* result = &stats->count.results[0];
    TEST_ASSERT_TRUE(result->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result->logicalLines);
    TEST_ASSERT_EQUAL_INT(188, result->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->words);
    TEST_ASSERT_EQUAL_INT(0, result->characters);
    TEST_ASSERT_EQUAL_INT(4709, result->sourceSize);
    TEST_ASSERT_TRUE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result->state.errorCode);
    TEST_ASSERT_NULL(result->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsOnlyWords(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java/Sample.java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .operations = RCN_OPT_COUNT_WORDS,
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(494, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(4709, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(494, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(4709, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(1, stats->count.size);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, stats->count.files[0].status);
    RcnCountResultGroup* result = &stats->count.results[0];
    TEST_ASSERT_TRUE(result->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->physicalLines);
    TEST_ASSERT_EQUAL_INT(494, result->words);
    TEST_ASSERT_EQUAL_INT(0, result->characters);
    TEST_ASSERT_EQUAL_INT(4709, result->sourceSize);
    TEST_ASSERT_TRUE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result->state.errorCode);
    TEST_ASSERT_NULL(result->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsOnlyCharacters(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java/Sample.java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .operations = RCN_OPT_COUNT_CHARACTERS,
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(4709, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(4709, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(4709, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(4709, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(1, stats->count.size);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, stats->count.files[0].status);
    RcnCountResultGroup* result = &stats->count.results[0];
    TEST_ASSERT_TRUE(result->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result->words);
    TEST_ASSERT_EQUAL_INT(4709, result->characters);
    TEST_ASSERT_EQUAL_INT(4709, result->sourceSize);
    TEST_ASSERT_TRUE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result->state.errorCode);
    TEST_ASSERT_NULL(result->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsOnlyJavaSources(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .formats = RCN_OPT_LANG_JAVA
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(16, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(21, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(39, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(450, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(450, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(16, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(21, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(39, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(450, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(450, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(4, stats->count.size);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    RcnSourceFile* file = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Source.java", file->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file->status);
    RcnCountResultGroup* result = &stats->count.results[0];
    TEST_ASSERT_TRUE(result->isProcessed);
    TEST_ASSERT_EQUAL_INT(16, result->logicalLines);
    TEST_ASSERT_EQUAL_INT(21, result->physicalLines);
    TEST_ASSERT_EQUAL_INT(39, result->words);
    TEST_ASSERT_EQUAL_INT(450, result->characters);
    TEST_ASSERT_EQUAL_INT(450, result->sourceSize);
    TEST_ASSERT_TRUE(result->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result->state.errorCode);
    TEST_ASSERT_NULL(result->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsOnlyUnformattedTextSources(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .formats = RCN_OPT_TEXT_UNFORMATTED
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(8, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(22, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(122, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(122, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(8, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(22, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(00, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(122, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(122, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(4, stats->count.size);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    RcnSourceFile* file = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("text.txt", file->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file->status);
    RcnCountResultGroup* result1 = &stats->count.results[0];
    TEST_ASSERT_FALSE(result1->isProcessed);
    RcnCountResultGroup* result2 = &stats->count.results[1];
    TEST_ASSERT_FALSE(result2->isProcessed);
    RcnCountResultGroup* result3 = &stats->count.results[2];
    TEST_ASSERT_TRUE(result3->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result3->logicalLines);
    TEST_ASSERT_EQUAL_INT(8, result3->physicalLines);
    TEST_ASSERT_EQUAL_INT(22, result3->words);
    TEST_ASSERT_EQUAL_INT(122, result3->characters);
    TEST_ASSERT_EQUAL_INT(122, result3->sourceSize);
    TEST_ASSERT_TRUE(result3->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result3->state.errorCode);
    TEST_ASSERT_NULL(result3->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsAllOperationsWithAllDefaultOptions(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {0};
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(34, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(60, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(129, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(1079, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(1079, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_MARKDOWN]);
    TEST_ASSERT_EQUAL_INT(16, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(18, stats->logicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(8, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(10, stats->physicalLines[RCN_TEXT_MARKDOWN]);
    TEST_ASSERT_EQUAL_INT(21, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(21, stats->physicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(22, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(25, stats->words[RCN_TEXT_MARKDOWN]);
    TEST_ASSERT_EQUAL_INT(39, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(43, stats->words[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(122, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(138, stats->characters[RCN_TEXT_MARKDOWN]);
    TEST_ASSERT_EQUAL_INT(450, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(369, stats->characters[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(122, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(138, stats->sourceSize[RCN_TEXT_MARKDOWN]);
    TEST_ASSERT_EQUAL_INT(450, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(369, stats->sourceSize[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(4, stats->count.size);
    TEST_ASSERT_EQUAL_INT(4, stats->count.sizeProcessed);
    RcnSourceFile* javaFile = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Source.java", javaFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, javaFile->status);
    RcnSourceFile* cFile = &stats->count.files[1];
    TEST_ASSERT_EQUAL_STRING("source.c", cFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, cFile->status);
    RcnSourceFile* textFile = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("text.txt", textFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, textFile->status);
    RcnSourceFile* mdFile = &stats->count.files[3];
    TEST_ASSERT_EQUAL_STRING("text2.md", mdFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, mdFile->status);
    RcnCountResultGroup* javaResult = &stats->count.results[0];
    TEST_ASSERT_TRUE(javaResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(16, javaResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(21, javaResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(39, javaResult->words);
    TEST_ASSERT_EQUAL_INT(450, javaResult->characters);
    TEST_ASSERT_EQUAL_INT(450, javaResult->sourceSize);
    TEST_ASSERT_TRUE(javaResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, javaResult->state.errorCode);
    TEST_ASSERT_NULL(javaResult->state.errorMessage);
    RcnCountResultGroup* cResult = &stats->count.results[1];
    TEST_ASSERT_TRUE(cResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(18, cResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(21, cResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(43, cResult->words);
    TEST_ASSERT_EQUAL_INT(369, cResult->characters);
    TEST_ASSERT_EQUAL_INT(369, cResult->sourceSize);
    TEST_ASSERT_TRUE(cResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, cResult->state.errorCode);
    TEST_ASSERT_NULL(cResult->state.errorMessage);
    RcnCountResultGroup* textResult = &stats->count.results[2];
    TEST_ASSERT_TRUE(textResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, textResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(8, textResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(22, textResult->words);
    TEST_ASSERT_EQUAL_INT(122, textResult->characters);
    TEST_ASSERT_EQUAL_INT(122, textResult->sourceSize);
    TEST_ASSERT_TRUE(textResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, textResult->state.errorCode);
    TEST_ASSERT_NULL(textResult->state.errorMessage);
    RcnCountResultGroup* mdResult = &stats->count.results[3];
    TEST_ASSERT_TRUE(mdResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, mdResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(10, mdResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(25, mdResult->words);
    TEST_ASSERT_EQUAL_INT(138, mdResult->characters);
    TEST_ASSERT_EQUAL_INT(138, mdResult->sourceSize);
    TEST_ASSERT_TRUE(mdResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, mdResult->state.errorCode);
    TEST_ASSERT_NULL(mdResult->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsOperationsWithExplicitOptions(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .operations = (
              RCN_OPT_COUNT_LOGICAL_LINES
            | RCN_OPT_COUNT_PHYSICAL_LINES
            | RCN_OPT_COUNT_WORDS
            | RCN_OPT_COUNT_CHARACTERS
        ),
        .formats = RCN_OPT_LANG_JAVA | RCN_OPT_TEXT_UNFORMATTED
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(16, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(29, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(61, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(572, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(572, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(16, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(8, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(21, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(22, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(39, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(122, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(450, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(122, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(450, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(4, stats->count.size);
    TEST_ASSERT_EQUAL_INT(2, stats->count.sizeProcessed);
    RcnSourceFile* javaFile = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Source.java", javaFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, javaFile->status);
    RcnSourceFile* cFile = &stats->count.files[1];
    TEST_ASSERT_EQUAL_STRING("source.c", cFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, cFile->status);
    RcnSourceFile* textFile = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("text.txt", textFile->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, textFile->status);
    RcnCountResultGroup* javaResult = &stats->count.results[0];
    TEST_ASSERT_TRUE(javaResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(16, javaResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(21, javaResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(39, javaResult->words);
    TEST_ASSERT_EQUAL_INT(450, javaResult->characters);
    TEST_ASSERT_EQUAL_INT(450, javaResult->sourceSize);
    TEST_ASSERT_TRUE(javaResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, javaResult->state.errorCode);
    TEST_ASSERT_NULL(javaResult->state.errorMessage);
    RcnCountResultGroup* cResult = &stats->count.results[1];
    TEST_ASSERT_FALSE(cResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, cResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, cResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, cResult->words);
    TEST_ASSERT_EQUAL_INT(0, cResult->characters);
    TEST_ASSERT_EQUAL_INT(0, cResult->sourceSize);
    TEST_ASSERT_FALSE(cResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, cResult->state.errorCode);
    TEST_ASSERT_NULL(cResult->state.errorMessage);
    RcnCountResultGroup* textResult = &stats->count.results[2];
    TEST_ASSERT_TRUE(textResult->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, textResult->logicalLines);
    TEST_ASSERT_EQUAL_INT(8, textResult->physicalLines);
    TEST_ASSERT_EQUAL_INT(22, textResult->words);
    TEST_ASSERT_EQUAL_INT(122, textResult->characters);
    TEST_ASSERT_EQUAL_INT(122, textResult->sourceSize);
    TEST_ASSERT_TRUE(textResult->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, textResult->state.errorCode);
    TEST_ASSERT_NULL(textResult->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsTwoOperationsOnlyOneLanguage(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    uint32_t twoOps = (
        RCN_OPT_COUNT_LOGICAL_LINES | RCN_OPT_COUNT_PHYSICAL_LINES
    );
    uint32_t onlyJava = RCN_OPT_LANG_JAVA;
    RcnStatOptions options = {
        .operations = twoOps,
        .formats = onlyJava
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(312, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(483, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(16194, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(312, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(483, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(16194, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_LANG_C]);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* file1 = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Sample.java", file1->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    RcnSourceFile* file2 = &stats->count.files[1];
    TEST_ASSERT_EQUAL_STRING("SampleAnnotated.java", file2->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file2->status);
    RcnSourceFile* file3 = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("SampleMinFormatting.java", file3->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file3->status);
    RcnCountResultGroup* result1 = &stats->count.results[0];
    TEST_ASSERT_TRUE(result1->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result1->logicalLines);
    TEST_ASSERT_EQUAL_INT(188, result1->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result1->words);
    TEST_ASSERT_EQUAL_INT(0, result1->characters);
    TEST_ASSERT_EQUAL_INT(4709, result1->sourceSize);
    TEST_ASSERT_TRUE(result1->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result1->state.errorCode);
    TEST_ASSERT_NULL(result1->state.errorMessage);
    RcnCountResultGroup* result2 = &stats->count.results[1];
    TEST_ASSERT_TRUE(result2->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result2->logicalLines);
    TEST_ASSERT_EQUAL_INT(188, result2->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result2->words);
    TEST_ASSERT_EQUAL_INT(0, result2->characters);
    TEST_ASSERT_EQUAL_INT(7424, result2->sourceSize);
    TEST_ASSERT_TRUE(result2->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result2->state.errorCode);
    TEST_ASSERT_NULL(result2->state.errorMessage);
    RcnCountResultGroup* result3 = &stats->count.results[2];
    TEST_ASSERT_TRUE(result3->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result3->logicalLines);
    TEST_ASSERT_EQUAL_INT(107, result3->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result3->words);
    TEST_ASSERT_EQUAL_INT(0, result3->characters);
    TEST_ASSERT_EQUAL_INT(4061, result3->sourceSize);
    TEST_ASSERT_TRUE(result3->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result3->state.errorCode);
    TEST_ASSERT_NULL(result3->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsExcludeJavaSkipsAll(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .formats = RCN_OPT_TEXT_UNFORMATTED // excludes Java
    };
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(0, stats->totalWords);
    TEST_ASSERT_EQUAL_INT(0, stats->totalCharacters);
    TEST_ASSERT_EQUAL_INT(0, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->logicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->physicalLines[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->words[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->characters[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_TEXT_UNFORMATTED]);
    TEST_ASSERT_EQUAL_INT(0, stats->sourceSize[RCN_LANG_JAVA]);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(0, stats->count.sizeProcessed);
    RcnSourceFile* file1 = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Sample.java", file1->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    RcnSourceFile* file2 = &stats->count.files[1];
    TEST_ASSERT_EQUAL_STRING("SampleAnnotated.java", file2->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file2->status);
    RcnSourceFile* file3 = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("SampleMinFormatting.java", file3->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file3->status);
    RcnCountResultGroup* result1 = &stats->count.results[0];
    TEST_ASSERT_FALSE(result1->isProcessed);
    TEST_ASSERT_FALSE(result1->state.ok);
    TEST_ASSERT_EQUAL_INT(0, result1->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result1->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result1->words);
    TEST_ASSERT_EQUAL_INT(0, result1->characters);
    TEST_ASSERT_EQUAL_INT(0, result1->sourceSize);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result1->state.errorCode);
    TEST_ASSERT_NULL(result1->state.errorMessage);
    RcnCountResultGroup* result2 = &stats->count.results[1];
    TEST_ASSERT_FALSE(result2->isProcessed);
    TEST_ASSERT_FALSE(result2->state.ok);
    TEST_ASSERT_EQUAL_INT(0, result2->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result2->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result2->words);
    TEST_ASSERT_EQUAL_INT(0, result2->characters);
    TEST_ASSERT_EQUAL_INT(0, result2->sourceSize);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result2->state.errorCode);
    TEST_ASSERT_NULL(result2->state.errorMessage);
    RcnCountResultGroup* result3 = &stats->count.results[2];
    TEST_ASSERT_FALSE(result3->isProcessed);
    TEST_ASSERT_FALSE(result3->state.ok);
    TEST_ASSERT_EQUAL_INT(0, result3->logicalLines);
    TEST_ASSERT_EQUAL_INT(0, result3->physicalLines);
    TEST_ASSERT_EQUAL_INT(0, result3->words);
    TEST_ASSERT_EQUAL_INT(0, result3->characters);
    TEST_ASSERT_EQUAL_INT(0, result3->sourceSize);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result3->state.errorCode);
    TEST_ASSERT_NULL(result3->state.errorMessage);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsWithKeepFileContentOptionDeactivated(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .keepFileContent = false
    };
    rcnCount(stats, options);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* file1 = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Sample.java", file1->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    TEST_ASSERT_FALSE(file1->isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file1->content.size);
    TEST_ASSERT_NULL(file1->content.text);
    RcnSourceFile* file2 = &stats->count.files[1];
    TEST_ASSERT_EQUAL_STRING("SampleAnnotated.java", file2->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file2->status);
    TEST_ASSERT_FALSE(file2->isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file2->content.size);
    TEST_ASSERT_NULL(file2->content.text);
    RcnSourceFile* file3 = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("SampleMinFormatting.java", file3->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file3->status);
    TEST_ASSERT_FALSE(file3->isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file3->content.size);
    TEST_ASSERT_NULL(file3->content.text);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsWithKeepFileContentOptionActivated(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .keepFileContent = true
    };
    rcnCount(stats, options);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* file1 = &stats->count.files[0];
    TEST_ASSERT_EQUAL_STRING("Sample.java", file1->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    TEST_ASSERT_TRUE(file1->isContentRead);
    TEST_ASSERT_EQUAL_INT(4709, file1->content.size);
    TEST_ASSERT_NOT_NULL(file1->content.text);
    RcnSourceFile* expected = newSourceFile(file1->path);
    readSourceFileContent(expected);
    TEST_ASSERT_EQUAL_STRING(expected->content.text, file1->content.text);
    freeSourceFile(expected);
    expected = NULL;
    RcnSourceFile* file2 = &stats->count.files[1];
    TEST_ASSERT_EQUAL_STRING("SampleAnnotated.java", file2->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file2->status);
    TEST_ASSERT_TRUE(file2->isContentRead);
    TEST_ASSERT_EQUAL_INT(7424, file2->content.size);
    TEST_ASSERT_NOT_NULL(file2->content.text);
    expected = newSourceFile(file2->path);
    readSourceFileContent(expected);
    TEST_ASSERT_EQUAL_STRING(expected->content.text, file2->content.text);
    freeSourceFile(expected);
    expected = NULL;
    RcnSourceFile* file3 = &stats->count.files[2];
    TEST_ASSERT_EQUAL_STRING("SampleMinFormatting.java", file3->name);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file3->status);
    TEST_ASSERT_TRUE(file3->isContentRead);
    TEST_ASSERT_EQUAL_INT(4061, file3->content.size);
    TEST_ASSERT_NOT_NULL(file3->content.text);
    expected = newSourceFile(file3->path);
    readSourceFileContent(expected);
    TEST_ASSERT_EQUAL_STRING(expected->content.text, file3->content.text);
    freeSourceFile(expected);
    expected = NULL;
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsWithStopOnErrorOptionDeactivated(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .stopOnError = false,
        .keepFileContent = true // used to track read ops
    };
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    // Mess up file path of the 2/3 file to trigger a not found error
    char* pathOfWrongFile = stats->count.files[1].path;
    pathOfWrongFile[strlen(pathOfWrongFile)-6] = 'X';
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_EQUAL_INT(2, stats->count.sizeProcessed);
    RcnSourceFile* file1 = &stats->count.files[0];
    RcnCountResultGroup* result1 = &stats->count.results[0];
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    TEST_ASSERT_TRUE(file1->isContentRead);
    TEST_ASSERT_TRUE(result1->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result1->state.errorCode);
    TEST_ASSERT_TRUE(result1->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result1->logicalLines);
    RcnSourceFile* file2 = &stats->count.files[1];
    RcnCountResultGroup* result2 = &stats->count.results[1];
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_FILE_NOT_FOUND, file2->status);
    TEST_ASSERT_FALSE(file2->isContentRead);
    TEST_ASSERT_FALSE(result2->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result2->state.errorCode);
    TEST_ASSERT_FALSE(result2->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result2->logicalLines);
    RcnSourceFile* file3 = &stats->count.files[2];
    RcnCountResultGroup* result3 = &stats->count.results[2];
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file3->status);
    TEST_ASSERT_TRUE(file3->isContentRead);
    TEST_ASSERT_TRUE(result3->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result3->state.errorCode);
    TEST_ASSERT_TRUE(result3->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result3->logicalLines);
    rcnFreeCountStatistics(stats);
}

void testCountStatisticsWithStopOnErrorOptionActivated(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java";
    RcnCountStatistics* stats = rcnCreateCountStatistics(path);
    RcnStatOptions options = {
        .stopOnError = true,
        .keepFileContent = true // used to track read ops
    };
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    // Mess up file path of the 2/3 file to trigger a not found error
    char* pathOfWrongFile = stats->count.files[1].path;
    pathOfWrongFile[strlen(pathOfWrongFile)-6] = 'X';
    rcnCount(stats, options);
    TEST_ASSERT_FALSE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, stats->state.errorCode);
    TEST_ASSERT_EQUAL_INT(1, stats->count.sizeProcessed);
    RcnSourceFile* file1 = &stats->count.files[0];
    RcnCountResultGroup* result1 = &stats->count.results[0];
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    TEST_ASSERT_TRUE(file1->isContentRead);
    TEST_ASSERT_TRUE(result1->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result1->state.errorCode);
    TEST_ASSERT_TRUE(result1->isProcessed);
    TEST_ASSERT_EQUAL_INT(104, result1->logicalLines);
    RcnSourceFile* file2 = &stats->count.files[1];
    RcnCountResultGroup* result2 = &stats->count.results[1];
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_FILE_NOT_FOUND, file2->status);
    TEST_ASSERT_FALSE(file2->isContentRead);
    TEST_ASSERT_FALSE(result2->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result2->state.errorCode);
    TEST_ASSERT_FALSE(result2->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result2->logicalLines);
    RcnSourceFile* file3 = &stats->count.files[2];
    RcnCountResultGroup* result3 = &stats->count.results[2];
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file3->status);
    TEST_ASSERT_FALSE(file3->isContentRead);
    TEST_ASSERT_FALSE(result3->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result3->state.errorCode);
    TEST_ASSERT_FALSE(result3->isProcessed);
    TEST_ASSERT_EQUAL_INT(0, result3->logicalLines);
    rcnFreeCountStatistics(stats);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCountStatisticsOnlyLogicalLines);
    RUN_TEST(testCountStatisticsOnlyPhysicalLines);
    RUN_TEST(testCountStatisticsOnlyWords);
    RUN_TEST(testCountStatisticsOnlyCharacters);
    RUN_TEST(testCountStatisticsOnlyJavaSources);
    RUN_TEST(testCountStatisticsOnlyUnformattedTextSources);
    RUN_TEST(testCountStatisticsAllOperationsWithAllDefaultOptions);
    RUN_TEST(testCountStatisticsOperationsWithExplicitOptions);
    RUN_TEST(testCountStatisticsTwoOperationsOnlyOneLanguage);
    RUN_TEST(testCountStatisticsExcludeJavaSkipsAll);
    RUN_TEST(testCountStatisticsWithKeepFileContentOptionDeactivated);
    RUN_TEST(testCountStatisticsWithKeepFileContentOptionActivated);
    RUN_TEST(testCountStatisticsWithStopOnErrorOptionDeactivated);
    RUN_TEST(testCountStatisticsWithStopOnErrorOptionActivated);
    return UNITY_END();
}
