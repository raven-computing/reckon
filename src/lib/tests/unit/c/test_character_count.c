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

// NOLINTBEGIN(readability-magic-numbers)

void testCharacterCountWithZeroSizeInputSucceeds(void) {
    RcnSourceText source = {
        .text = "",
        .size = 0
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCharacterCountWithInvalidInputFails(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 1
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Text input must not be NULL",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCharacterCountWithTooLargeInputFails(void) {
    RcnSourceText source = {
        .text = "AAAAAA....AAAA",
        .size = 0x000000FFFFFFFFFF
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INPUT_TOO_LARGE, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Input exceeds maximum supported size",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCharacterCountIsCorrect(void) {
    char* text = "abc\ndef\n";
    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(8, result.count);
}

void testCharacterCountForSourceCodeFile(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/java/Sample.java";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountCharacters(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(4709, result.count);
    freeSourceFile(file);
}

void testCharacterCountUnformattedTextFile(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/mixed/text.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountCharacters(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(122, result.count);
    freeSourceFile(file);
}

void testCharacterCountEncodedTextUTF8(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/encodings/text_UTF_8.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountCharacters(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(108, result.count);
    freeSourceFile(file);
}

void testCharacterCountEncodedTextUTF8WithBOM(void) {
    char* path = RECKON_TEST_PATH_RES_BASE"/encodings/text_UTF_8_with_BOM.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountCharacters(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(103, result.count);
    freeSourceFile(file);
}

void testCharacterCountWithStrayUTF16LEBOM(void) {
    unsigned char textUtf16le[] = {
        0xff,0xfe,'a',0x00,'b',0x00,'c',0x00,0xff,
        0xfe,'d',0x00,'e',0x00,'\n',0x00,'t',0x00
    };
    char* textUtf16lePtr = (char*) textUtf16le;
    RcnSourceText source = {
        .text = textUtf16lePtr,
        .size = sizeof(textUtf16le)
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(7, result.count);
}

void testCharacterCountWithStrayUTF16BEBOM(void) {
    unsigned char textUtf16be[] = {
        0xfe,0xff,'a',0x00,'b',0x00,'c',0x00,0xfe,
        0xff,'d',0x00,'e',0x00,'\n',0x00,'t',0x00
    };
    char* textUtf16bePtr = (char*) textUtf16be;
    RcnSourceText source = {
        .text = textUtf16bePtr,
        .size = sizeof(textUtf16be)
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(7, result.count);
}

void testCharacterCountWithStrayUTF16LowSurrogate(void) {
    unsigned char textUtf16be[] = {
        0xfe,0xff,'a',0x00,'b',0x00,'c',0x00,0xdc,
        0xaa,'d',0x00,'e',0x00,'\n',0x00,'t',0x00
    };
    char* textUtf16bePtr = (char*) textUtf16be;
    RcnSourceText source = {
        .text = textUtf16bePtr,
        .size = sizeof(textUtf16be)
    };
    RcnCountResult result = rcnCountCharacters(source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(7, result.count);
}

void testCharacterCountEncodedTextUTF16BE(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/encodings/text_UTF_16BE.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountCharacters(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(92, result.count);
    freeSourceFile(file);
}

void testCharacterCountEncodedTextUTF16LE(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/encodings/text_UTF_16LE.txt";
    RcnSourceFile* file = newSourceFile(path);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountCharacters(file->content);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(92, result.count);
    freeSourceFile(file);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCharacterCountWithZeroSizeInputSucceeds);
    RUN_TEST(testCharacterCountWithInvalidInputFails);
    RUN_TEST(testCharacterCountWithTooLargeInputFails);
    RUN_TEST(testCharacterCountIsCorrect);
    RUN_TEST(testCharacterCountForSourceCodeFile);
    RUN_TEST(testCharacterCountUnformattedTextFile);
    RUN_TEST(testCharacterCountEncodedTextUTF8);
    RUN_TEST(testCharacterCountEncodedTextUTF8WithBOM);
    RUN_TEST(testCharacterCountWithStrayUTF16LEBOM);
    RUN_TEST(testCharacterCountWithStrayUTF16BEBOM);
    RUN_TEST(testCharacterCountWithStrayUTF16LowSurrogate);
    RUN_TEST(testCharacterCountEncodedTextUTF16BE);
    RUN_TEST(testCharacterCountEncodedTextUTF16LE);
    return UNITY_END();
}
