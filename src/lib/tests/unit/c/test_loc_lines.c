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
#define TEST_FILE_SOURCE_UTF_8_BOM TEST_DIR_ENC "/Source_UTF_8_with_BOM.java"
#define TEST_FILE_SOURCE_UTF_16_LE TEST_DIR_ENC "/Source_UTF_16LE.java"
#define TEST_FILE_SOURCE_UTF_16_BE TEST_DIR_ENC "/Source_UTF_16BE.java"

void setUp(void) { }

void tearDown(void) { }

void testCodeLineCountIsCorrect(void) {
    char *text =
        "public class Test {\n"
        "    public static void main(String[] args) {\n"
        "        // This is a comment\n"
        "        System.out.println(\"This is actual code!\");\n"
        "        /*A block comment \n"
        "        spanning \n"
        "        multiple lines. */\n"
        "    }\n"
        "}\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(5, result.count);
}

void testCodeLineCountWithInvalidInputFails(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 1
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Source code input must not be NULL",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCodeLineCountWithZeroSizeInputSucceeds(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 0
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCodeLineCountWithEncodedSourceUTF8(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_8);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
}

void testCodeLineCountWithEncodedSourceUTF8WithBOM(void) {
    RcnSourceFile *file = newSourceFile(TEST_FILE_SOURCE_UTF_8_BOM);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
}

void testCodeLineCountWithEncodedSourceUTF16LE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_LE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
}

void testCodeLineCountWithEncodedSourceUTF16BE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_BE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
}

void testCodeLineCountWithLastLineNotEndingWithNewline(void) {
    char *text =
        "public class Test {\n"
        "    public static void main(String[] args) {\n"
        "        // This is a comment\n"
        "        System.out.println(\"This is code!\");\n"
        "    }\n"
        "}";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };

    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(5, result.count);
}

void testCodeLineCountWithOnlyInputUTF16LEBOM(void) {
    char* text = "\xff\xfe";
    RcnSourceText source = {
        .text = text,
        .size = 2
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCodeLineCountIsCorrect);
    RUN_TEST(testCodeLineCountWithInvalidInputFails);
    RUN_TEST(testCodeLineCountWithZeroSizeInputSucceeds);
    RUN_TEST(testCodeLineCountWithEncodedSourceUTF8);
    RUN_TEST(testCodeLineCountWithEncodedSourceUTF8WithBOM);
    RUN_TEST(testCodeLineCountWithEncodedSourceUTF16LE);
    RUN_TEST(testCodeLineCountWithEncodedSourceUTF16BE);
    RUN_TEST(testCodeLineCountWithLastLineNotEndingWithNewline);
    RUN_TEST(testCodeLineCountWithOnlyInputUTF16LEBOM);
    return UNITY_END();
}
