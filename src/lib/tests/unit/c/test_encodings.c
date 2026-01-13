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
#include "evaluation.h"
#include "fileio.h"

#define TEST_RES_DIR RECKON_TEST_PATH_RES_BASE "/encodings"
#define TEST_FILE_TEXT_UTF_8 TEST_RES_DIR "/text_UTF_8.txt"
#define TEST_FILE_TEXT_UTF_8_BOM TEST_RES_DIR "/text_UTF_8_with_BOM.txt"
#define TEST_FILE_TEXT_UTF_16_LE TEST_RES_DIR "/text_UTF_16LE.txt"
#define TEST_FILE_TEXT_UTF_16_BE TEST_RES_DIR "/text_UTF_16BE.txt"

void setUp(void) { }

void tearDown(void) { }

void testTextEncodingIsDetectedCorrectlyWithDefaultUTF8(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_8);
    readSourceFileContent(file);
    TextEncoding encoding = detectEncoding(file->content);
    TEST_ASSERT_EQUAL_INT(TextEncodingUTF8, encoding);
    freeSourceFile(file);
}

void testTextEncodingIsDetectedCorrectlyWithBOMforUTF8(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_8_BOM);
    readSourceFileContent(file);
    TextEncoding encoding = detectEncoding(file->content);
    TEST_ASSERT_EQUAL_INT(TextEncodingUTF8, encoding);
    freeSourceFile(file);
}

void testTextEncodingIsDetectedCorrectlyUTF16LE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_16_LE);
    readSourceFileContent(file);
    TextEncoding encoding = detectEncoding(file->content);
    TEST_ASSERT_EQUAL_INT(TextEncodingUTF16LE, encoding);
    freeSourceFile(file);
}

void testTextEncodingIsDetectedCorrectlyUTF16BE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_TEXT_UTF_16_BE);
    readSourceFileContent(file);
    TextEncoding encoding = detectEncoding(file->content);
    TEST_ASSERT_EQUAL_INT(TextEncodingUTF16BE, encoding);
    freeSourceFile(file);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testTextEncodingIsDetectedCorrectlyWithDefaultUTF8);
    RUN_TEST(testTextEncodingIsDetectedCorrectlyWithBOMforUTF8);
    RUN_TEST(testTextEncodingIsDetectedCorrectlyUTF16LE);
    RUN_TEST(testTextEncodingIsDetectedCorrectlyUTF16BE);
    return UNITY_END();
}
