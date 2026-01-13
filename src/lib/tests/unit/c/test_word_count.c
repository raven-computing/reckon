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
#include "fileio.h"

void setUp(void) { }

void tearDown(void) { }

void testWordCountIsCorrect(void) {
    char* text = "one two three";
    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountWords(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
}

void testWordCountWithInvalidInputFails(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 1
    };
    RcnCountResult result = rcnCountWords(source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Text input must not be NULL",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testWordCountWithTooLargeTextInputFails(void) {
    RcnSourceText source = {
        .text = "AAAAAA....AAAA",
        .size = 0x000000FFFFFFFFFF // NOLINT
    };
    RcnCountResult result = rcnCountWords(source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INPUT_TOO_LARGE, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Input exceeds maximum supported size",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testWordCountWithZeroLengthInputSucceeds(void) {
    RcnSourceText source = {
        .text = "",
        .size = 0
    };
    RcnCountResult result = rcnCountWords(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testWordCountForSourceCodeFile(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed/Source.java";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountWords(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(39, result.count);
    freeSourceFile(file);
}

void testWordCountUnformattedTextFile(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed/text.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountWords(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(22, result.count);
    freeSourceFile(file);
}

void testWordCountEncodedTextUTF8(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/encodings/text_UTF_8.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountWords(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(22, result.count);
    freeSourceFile(file);
}

void testWordCountEncodedTextUTF8WithBOM(void) {
    char* path = RECKON_TEST_PATH_RES_BASE"/encodings/text_UTF_8_with_BOM.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountWords(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(22, result.count);
    freeSourceFile(file);
}

void testWordCountEncodedTextUTF16BE(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/encodings/text_UTF_16BE.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountWords(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
    freeSourceFile(file);
}

void testWordCountEncodedTextUTF16LE(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/encodings/text_UTF_16LE.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountWords(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
    freeSourceFile(file);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testWordCountIsCorrect);
    RUN_TEST(testWordCountWithInvalidInputFails);
    RUN_TEST(testWordCountWithTooLargeTextInputFails);
    RUN_TEST(testWordCountWithZeroLengthInputSucceeds);
    RUN_TEST(testWordCountForSourceCodeFile);
    RUN_TEST(testWordCountUnformattedTextFile);
    RUN_TEST(testWordCountEncodedTextUTF8);
    RUN_TEST(testWordCountEncodedTextUTF8WithBOM);
    RUN_TEST(testWordCountEncodedTextUTF16BE);
    RUN_TEST(testWordCountEncodedTextUTF16LE);
    return UNITY_END();
}
