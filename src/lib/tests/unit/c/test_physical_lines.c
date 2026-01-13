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

#define TEST_DIR_ENC RECKON_TEST_PATH_RES_BASE "/encodings"
#define TEST_FILE_SOURCE_UTF_8 TEST_DIR_ENC "/Source_UTF_8.java"
#define TEST_FILE_TEXT_UTF_8_BOM TEST_DIR_ENC "/text_UTF_8_with_BOM.txt"
#define TEST_FILE_SOURCE_UTF_16_LE TEST_DIR_ENC "/Source_UTF_16LE.java"
#define TEST_FILE_SOURCE_UTF_16_BE TEST_DIR_ENC "/Source_UTF_16BE.java"
#define TEST_FILE_TEXT_UTF_16_LE_NONL TEST_DIR_ENC "/text_UTF_16LE_noNLend.txt"
#define TEST_FILE_TEXT_UTF_16_BE_NONL TEST_DIR_ENC "/text_UTF_16BE_noNLend.txt"

void setUp(void) { }

void tearDown(void) { }

void testPhysicalLineCountIsCorrect(void) {
    char* text =
        "a\n"
        "\n"
        "b c d\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
}

void testPhysicalLineCountWithInvalidInputFails(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 1
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Text input must not be NULL",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testPhysicalLineCountWithZeroSizeInputSucceeds(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 0
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testPhysicalLineCountWithEncodedSourceUTF8(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_8);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(23, result.count);
}

void testPhysicalLineCountWithEncodedSourceUTF8WithBOM(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_8_BOM);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
}

void testPhysicalLineCountWithEncodedSourceUTF16LE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_LE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(23, result.count);
}

void testPhysicalLineCountWithEncodedSourceUTF16BE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_BE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(23, result.count);
}

void testPhysicalLineCountWithLastLineNotEndingWithNewline(void) {
    char* text = (
        "this is a line A\n"
        "this is a line B\n"
        "this is a line C\n"
        "Another line here, that makes it 4. No NL here"
    );
    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(4, result.count);
}

void testPhysicalLineCountUTF16LEWithNoNewLineAtTheEnd(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_16_LE_NONL);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(2, result.count);
}

void testPhysicalLineCountUTF16BEWithNoNewLineAtTheEnd(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_16_BE_NONL);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(2, result.count);
}

void testPhysicalLineCountWithOneCharacterInput(void) {
    char* text = "A";
    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(1, result.count);
}

void testPhysicalLineCountWithOnlyInputUTF16LEBOM(void) {
    char* text = "\xff\xfe";
    RcnSourceText source = {
        .text = text,
        .size = 2
    };
    RcnCountResult result = rcnCountPhysicalLines(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testPhysicalLineCountIsCorrect);
    RUN_TEST(testPhysicalLineCountWithInvalidInputFails);
    RUN_TEST(testPhysicalLineCountWithZeroSizeInputSucceeds);
    RUN_TEST(testPhysicalLineCountWithEncodedSourceUTF8);
    RUN_TEST(testPhysicalLineCountWithEncodedSourceUTF8WithBOM);
    RUN_TEST(testPhysicalLineCountWithEncodedSourceUTF16LE);
    RUN_TEST(testPhysicalLineCountWithEncodedSourceUTF16BE);
    RUN_TEST(testPhysicalLineCountWithLastLineNotEndingWithNewline);
    RUN_TEST(testPhysicalLineCountUTF16LEWithNoNewLineAtTheEnd);
    RUN_TEST(testPhysicalLineCountUTF16BEWithNoNewLineAtTheEnd);
    RUN_TEST(testPhysicalLineCountWithOneCharacterInput);
    RUN_TEST(testPhysicalLineCountWithOnlyInputUTF16LEBOM);
    return UNITY_END();
}
