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

#define TEST_RES_DIR RECKON_TEST_PATH_RES_BASE "/javascript"
#define TEST_SAMPLE TEST_RES_DIR "/sample.js"
#define TEST_SAMPLE_ANNOTATED TEST_RES_DIR "/sample_annotated.js"
#define TEST_SAMPLE_MIN_FORMATTING TEST_RES_DIR "/sample_min_formatting.js"

char* javaScriptSourceWithSyntaxError =
    "export function broken() {\n"
    "    if (true) {\n"
    "        console.log(\"This is not good!\")\n"
    "    else {\n"
    "        return 0;\n"
    "    }\n"
    "}\n";

void setUp(void) { }

void tearDown(void) { }

void testJavaScriptLogicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_JAVASCRIPT,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(32, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testJavaScriptLogicalLineCountFailsWithSyntaxError(void) {
    RcnSourceText source = {
        .text = javaScriptSourceWithSyntaxError,
        .size = strlen(javaScriptSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountLogicalLines(RCN_LANG_JAVASCRIPT, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(0, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_SYNTAX_ERROR, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Syntax error detected in source code",
        result.state.errorMessage
    );
}

void testJavaScriptPhysicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(50, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testJavaScriptPhysicalLineCountWithSyntacticallyIncorrectCode(void) {
    RcnSourceText source = {
        .text = javaScriptSourceWithSyntaxError,
        .size = strlen(javaScriptSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(7, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testJavaScriptLogicalLineCountMarksAreCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(file);
    readSourceFileContent(goldenSample);
    RcnSourceText actual = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVASCRIPT,
        file->content
    );
    TEST_ASSERT_NOT_NULL(actual.text);
    TEST_ASSERT_EQUAL_INT(1689, actual.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, actual.text);
    freeSourceFile(file);
    freeSourceFile(goldenSample);
    free(actual.text);
}

void testJavaScriptLogicalLineCountMarksForFilePathInput(void) {
    char* path = TEST_SAMPLE;
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(goldenSample);
    RcnSourceText annotated = rcnMarkLogicalLinesInFile(path);
    TEST_ASSERT_NOT_NULL(annotated.text);
    TEST_ASSERT_EQUAL_INT(1689, annotated.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, annotated.text);
    freeSourceFile(goldenSample);
    rcnFreeSourceText(&annotated);
}

void testJavaScriptCountAllIsCorrect(void) {
    RcnCountStatistics* stats = rcnCreateCountStatistics(TEST_RES_DIR);
    RcnStatOptions options = {0};
    options.formats = RCN_OPT_LANG_JAVASCRIPT;
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(96, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(128, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(3365, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(96, stats->logicalLines[RCN_LANG_JAVASCRIPT]);
    TEST_ASSERT_EQUAL_INT(128, stats->physicalLines[RCN_LANG_JAVASCRIPT]);
    TEST_ASSERT_EQUAL_INT(3365, stats->sourceSize[RCN_LANG_JAVASCRIPT]);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* filelist = stats->count.files;
    TEST_ASSERT_EQUAL_STRING("sample.js", filelist[0].name);
    TEST_ASSERT_EQUAL_STRING("sample_annotated.js", filelist[1].name);
    TEST_ASSERT_EQUAL_STRING("sample_min_formatting.js", filelist[2].name);
    rcnFreeCountStatistics(stats);
}

void testJavaScriptLogicalLineCountForMinimizedFormattingIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE_MIN_FORMATTING);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_JAVASCRIPT,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(32, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testJavaScriptLogicalLineCountIsCorrect);
    RUN_TEST(testJavaScriptLogicalLineCountFailsWithSyntaxError);
    RUN_TEST(testJavaScriptPhysicalLineCountIsCorrect);
    RUN_TEST(testJavaScriptPhysicalLineCountWithSyntacticallyIncorrectCode);
    RUN_TEST(testJavaScriptLogicalLineCountMarksAreCorrect);
    RUN_TEST(testJavaScriptLogicalLineCountMarksForFilePathInput);
    RUN_TEST(testJavaScriptCountAllIsCorrect);
    RUN_TEST(testJavaScriptLogicalLineCountForMinimizedFormattingIsCorrect);
    return UNITY_END();
}
