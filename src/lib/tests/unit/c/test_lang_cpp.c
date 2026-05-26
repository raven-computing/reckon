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

#define TEST_RES_DIR RECKON_TEST_PATH_RES_BASE "/cpp"
#define TEST_SAMPLE TEST_RES_DIR "/Sample.cpp"
#define TEST_SAMPLE_ANNOTATED TEST_RES_DIR "/SampleAnnotated.cpp"
#define TEST_SAMPLE_MIN_FORMATTING TEST_RES_DIR "/SampleMinFormatting.cpp"

char* cppSourceWithSyntaxError =
    "class Hello {\n"
    "    void myFunc() {\n"
    "        std::cout << \"Hello!\"\n" // Missing semicolon
    "    }\n"
    "};\n";

void setUp(void) { }

void tearDown(void) { }

void testCppLogicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_CPP,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(197, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testCppLogicalLineCountIsLenientWithSyntaxError(void) {
    RcnSourceText source = {
        .text = cppSourceWithSyntaxError,
        .size = strlen(cppSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountLogicalLines(RCN_LANG_CPP, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(2, result.count);
}

void testCppLogicalLineCountStrictFailsWithSyntaxError(void) {
    RcnSourceText source = {
        .text = cppSourceWithSyntaxError,
        .size = strlen(cppSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountLogicalLinesStrict(RCN_LANG_CPP, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(0, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_SYNTAX_ERROR, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Syntax error detected in source code",
        result.state.errorMessage
    );
}

void testCppPhysicalLineCountIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(359, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testCppPhysicalLineCountWithSyntacticallyIncorrectCode(void) {
    RcnSourceText source = {
        .text = cppSourceWithSyntaxError,
        .size = strlen(cppSourceWithSyntaxError)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(5, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

void testCppLogicalLineCountMarksAreCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE);
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(file);
    readSourceFileContent(goldenSample);
    RcnSourceText actual = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_CPP,
        file->content
    );
    TEST_ASSERT_NOT_NULL(actual.text);
    TEST_ASSERT_EQUAL_INT(11706, actual.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, actual.text);
    freeSourceFile(file);
    freeSourceFile(goldenSample);
    free(actual.text);
}

void testCppLogicalLineCountMarksForFilePathInput(void) {
    char* path = TEST_SAMPLE;
    RcnSourceFile* goldenSample = newSourceFile(TEST_SAMPLE_ANNOTATED);
    readSourceFileContent(goldenSample);
    RcnSourceText annotated = rcnMarkLogicalLinesInFile(path);
    TEST_ASSERT_NOT_NULL(annotated.text);
    TEST_ASSERT_EQUAL_INT(11706, annotated.size);
    TEST_ASSERT_EQUAL_STRING(goldenSample->content.text, annotated.text);
    freeSourceFile(goldenSample);
    rcnFreeSourceText(&annotated);
}

void testCppCountAllIsCorrect(void) {
    RcnCountStatistics* stats = rcnCreateCountStatistics(TEST_RES_DIR);
    RcnStatOptions options = {0};
    options.formats = RCN_OPT_LANG_CPP;
    rcnCount(stats, options);
    TEST_ASSERT_TRUE(stats->state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, stats->state.errorCode);
    TEST_ASSERT_NULL(stats->state.errorMessage);
    TEST_ASSERT_EQUAL_INT(591, stats->totalLogicalLines);
    TEST_ASSERT_EQUAL_INT(918, stats->totalPhysicalLines);
    TEST_ASSERT_EQUAL_INT(24662, stats->totalSourceSize);
    TEST_ASSERT_EQUAL_INT(591, stats->logicalLines[RCN_LANG_CPP]);
    TEST_ASSERT_EQUAL_INT(918, stats->physicalLines[RCN_LANG_CPP]);
    TEST_ASSERT_EQUAL_INT(24662, stats->sourceSize[RCN_LANG_CPP]);
    TEST_ASSERT_EQUAL_INT(3, stats->count.size);
    TEST_ASSERT_EQUAL_INT(3, stats->count.sizeProcessed);
    RcnSourceFile* filelist = stats->count.files;
    TEST_ASSERT_EQUAL_STRING("Sample.cpp", filelist[0].name);
    TEST_ASSERT_EQUAL_STRING("SampleAnnotated.cpp", filelist[1].name);
    TEST_ASSERT_EQUAL_STRING("SampleMinFormatting.cpp", filelist[2].name);
    rcnFreeCountStatistics(stats);
}

void testCppLogicalLineCountForMinimizedFormattingIsCorrect(void) {
    RcnSourceFile* file = newSourceFile(TEST_SAMPLE_MIN_FORMATTING);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_CPP,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(197, result.count);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCppLogicalLineCountIsCorrect);
    RUN_TEST(testCppLogicalLineCountIsLenientWithSyntaxError);
    RUN_TEST(testCppLogicalLineCountStrictFailsWithSyntaxError);
    RUN_TEST(testCppPhysicalLineCountIsCorrect);
    RUN_TEST(testCppPhysicalLineCountWithSyntacticallyIncorrectCode);
    RUN_TEST(testCppLogicalLineCountMarksAreCorrect);
    RUN_TEST(testCppLogicalLineCountMarksForFilePathInput);
    RUN_TEST(testCppCountAllIsCorrect);
    RUN_TEST(testCppLogicalLineCountForMinimizedFormattingIsCorrect);
    return UNITY_END();
}
