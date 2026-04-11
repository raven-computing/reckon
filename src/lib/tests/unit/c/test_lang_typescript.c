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

#define TEST_RES_DIR RECKON_TEST_PATH_RES_BASE "/typescript"
#define TEST_SAMPLE TEST_RES_DIR "/sample.ts"
#define TEST_SAMPLE_ANNOTATED TEST_RES_DIR "/sample_annotated.ts"
#define TEST_SAMPLE_MIN_FORMATTING TEST_RES_DIR "/sample_min_formatting.ts"

char* typeScriptSourceWithSyntaxError =
    "export function broken() {\n"
    "    if (true) {\n"
    "        console.log(\"This is not good!\")\n"
    "    else {\n"
    "        return 0;\n"
    "    }\n"
    "}\n";

void setUp(void) { }

void tearDown(void) { }

void testTypeScriptLogicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_TYPESCRIPT,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(116, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testTypeScriptLogicalLineCountIsLenientWithSyntaxError(void) {
    RcnSourceText source = {
        .text = typeScriptSourceWithSyntaxError,
        .size = strlen(typeScriptSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountLogicalLines(RCN_LANG_TYPESCRIPT, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(4, result.count);
}

void testTypeScriptLogicalLineCountStrictFailsWithSyntaxError(void) {
    RcnSourceText source = {
        .text = typeScriptSourceWithSyntaxError,
        .size = strlen(typeScriptSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountLogicalLinesStrict(RCN_LANG_TYPESCRIPT, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(0, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_SYNTAX_ERROR, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Syntax error detected in source code",
        result.state.errorMessage
    );
}

void testTypeScriptPhysicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(286, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testTypeScriptPhysicalLineCountWithSyntacticallyIncorrectCode(void) {
    RcnSourceText source = {
        .text = typeScriptSourceWithSyntaxError,
        .size = strlen(typeScriptSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(7, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testTypeScriptLogicalLineCountMarksAreCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(file);
    readSourceFileContent(goldenSample);
    RcnSourceText actual = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_TYPESCRIPT,
        file->content
    );
    TEST_ASSERT_NOT_NULL(actual.text);
    TEST_ASSERT_EQUAL_INT(9284, actual.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, actual.text);
    freeSourceFile(file);
    freeSourceFile(goldenSample);
    free(actual.text);
}

void testTypeScriptLogicalLineCountMarksForFilePathInput(void) {
    char* path = TEST_SAMPLE;
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(goldenSample);
    RcnSourceText annotated = rcnMarkLogicalLinesInFile(path);
    TEST_ASSERT_NOT_NULL(annotated.text);
    TEST_ASSERT_EQUAL_INT(9284, annotated.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, annotated.text);
    freeSourceFile(goldenSample);
    rcnFreeSourceText(&annotated);
}

void testTypeScriptCountAllIsCorrect(void) {
    RcnCountStatistics* stats = rcnCreateCountStatistics(TEST_RES_DIR);
    RcnStatOptions options = {0};
    options.formats = RCN_OPT_LANG_TYPESCRIPT;
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(348, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(717, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(21042, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(348, stats->logicalLines[RCN_LANG_TYPESCRIPT]);
    TEST_ASSERT_EQUAL_INT(717, stats->physicalLines[RCN_LANG_TYPESCRIPT]);
    TEST_ASSERT_EQUAL_INT(21042, stats->sourceSize[RCN_LANG_TYPESCRIPT]);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* filelist = stats->count.files;
    TEST_ASSERT_EQUAL_STRING("sample.ts", filelist[0].name);
    TEST_ASSERT_EQUAL_STRING("sample_annotated.ts", filelist[1].name);
    TEST_ASSERT_EQUAL_STRING("sample_min_formatting.ts", filelist[2].name);
    rcnFreeCountStatistics(stats);
}

void testTypeScriptLogicalLineCountForMinimizedFormattingIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE_MIN_FORMATTING);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_TYPESCRIPT,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(116, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testTypeScriptLogicalLineCountIsCorrect);
    RUN_TEST(testTypeScriptLogicalLineCountIsLenientWithSyntaxError);
    RUN_TEST(testTypeScriptLogicalLineCountStrictFailsWithSyntaxError);
    RUN_TEST(testTypeScriptPhysicalLineCountIsCorrect);
    RUN_TEST(testTypeScriptPhysicalLineCountWithSyntacticallyIncorrectCode);
    RUN_TEST(testTypeScriptLogicalLineCountMarksAreCorrect);
    RUN_TEST(testTypeScriptLogicalLineCountMarksForFilePathInput);
    RUN_TEST(testTypeScriptCountAllIsCorrect);
    RUN_TEST(testTypeScriptLogicalLineCountForMinimizedFormattingIsCorrect);
    return UNITY_END();
}
