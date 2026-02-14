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

#include <string.h>

#include "unity.h"

#include "reckon/reckon.h"
#include "fileio.h"

#define TEST_RES_DIR RECKON_TEST_PATH_RES_BASE "/bash"
#define TEST_SAMPLE TEST_RES_DIR "/sample.sh"
#define TEST_SAMPLE_ANNOTATED TEST_RES_DIR "/sample_annotated.sh"
#define TEST_SAMPLE_MIN_FORMATTING TEST_RES_DIR "/sample_min_formatting.sh"

char* bashSourceWithSyntaxError =
    "#!/bin/bash\n"
    "if [[ -n \"x\" ]]; then\n"
    "    echo \"This is not good!\";\n"
    "\n";

void setUp(void) { }

void tearDown(void) { }

void testBashLogicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_BASH,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(159, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testBashLogicalLineCountFailsWithSyntaxError(void) {
    RcnSourceText source = {
        .text = bashSourceWithSyntaxError,
        .size = strlen(bashSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountLogicalLines(RCN_LANG_BASH, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(0, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_SYNTAX_ERROR, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Syntax error detected in source code",
        result.state.errorMessage
    );
}

void testBashPhysicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(196, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testBashPhysicalLineCountWithSyntacticallyIncorrectCode(void) {
    RcnSourceText source = {
        .text = bashSourceWithSyntaxError,
        .size = strlen(bashSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(4, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testBashLogicalLineCountMarksAreCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(file);
    readSourceFileContent(goldenSample);
    RcnSourceText actual = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_BASH,
        file->content
    );
    TEST_ASSERT_NOT_NULL(actual.text);
    TEST_ASSERT_EQUAL_INT(6466, actual.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, actual.text);
    freeSourceFile(file);
    freeSourceFile(goldenSample);
    free(actual.text);
}

void testBashLogicalLineCountMarksForFilePathInput(void) {
    char* path = TEST_SAMPLE;
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(goldenSample);
    RcnSourceText annotated = rcnMarkLogicalLinesInFile(path);
    TEST_ASSERT_NOT_NULL(annotated.text);
    TEST_ASSERT_EQUAL_INT(6466, annotated.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, annotated.text);
    freeSourceFile(goldenSample);
    rcnFreeSourceText(&annotated);
}

void testBashCountAllIsCorrect(void) {
    RcnCountStatistics* stats = rcnCreateCountStatistics(TEST_RES_DIR);
    RcnStatOptions options = {0};
    options.formats = RCN_OPT_LANG_BASH;
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(478, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(588, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(13900, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(478, stats->logicalLines[RCN_LANG_BASH]);
    TEST_ASSERT_EQUAL_INT(588, stats->physicalLines[RCN_LANG_BASH]);
    TEST_ASSERT_EQUAL_INT(13900, stats->sourceSize[RCN_LANG_BASH]);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* filelist = stats->count.files;
    TEST_ASSERT_EQUAL_STRING("sample.sh", filelist[0].name);
    TEST_ASSERT_EQUAL_STRING("sample_annotated.sh", filelist[1].name);
    TEST_ASSERT_EQUAL_STRING("sample_min_formatting.sh", filelist[2].name);
    rcnFreeCountStatistics(stats);
}

void testBashLogicalLineCountForMinimizedFormattingIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE_MIN_FORMATTING);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_BASH,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(159, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testBashLogicalLineCountIsCorrect);
    RUN_TEST(testBashLogicalLineCountFailsWithSyntaxError);
    RUN_TEST(testBashPhysicalLineCountIsCorrect);
    RUN_TEST(testBashPhysicalLineCountWithSyntacticallyIncorrectCode);
    RUN_TEST(testBashLogicalLineCountMarksAreCorrect);
    RUN_TEST(testBashLogicalLineCountMarksForFilePathInput);
    RUN_TEST(testBashCountAllIsCorrect);
    RUN_TEST(testBashLogicalLineCountForMinimizedFormattingIsCorrect);
    return UNITY_END();
}
