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

#define TEST_DIR_ENC RECKON_TEST_PATH_RES_BASE "/encodings"
#define TEST_FILE_SOURCE_UTF_8 TEST_DIR_ENC "/Source_UTF_8.java"
#define TEST_FILE_SOURCE_UTF_8_BOM TEST_DIR_ENC "/Source_UTF_8_with_BOM.java"
#define TEST_FILE_SOURCE_UTF_16_LE TEST_DIR_ENC "/Source_UTF_16LE.java"
#define TEST_FILE_SOURCE_UTF_16_BE TEST_DIR_ENC "/Source_UTF_16BE.java"

void setUp(void) { }

void tearDown(void) { }

// NOLINTBEGIN(readability-magic-numbers)

void testLogicalLineCountWithInvalidInputReturnsNull(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 0
    };
    RcnCountResult res = rcnCountLogicalLines(RCN_LANG_JAVA, source);
    TEST_ASSERT_FALSE(res.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INVALID_INPUT, res.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Source code input must not be NULL",
        res.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, res.count);
}

void testLogicalCountMarkWithInvalidInputReturnsNull(void) {
    char* text = NULL;
    RcnSourceText sourceCode = {
        .text = text,
        .size = 1
    };
    RcnSourceText actual = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVA,
        sourceCode
    );
    TEST_ASSERT_NULL(actual.text);
}

void testLogicalCountFailsWhenGivenInputWithUnknownLanguage(void) {
    char* code = (
        "class Data {\n"
        "    int bla() { }\n"
        "}\n"
    );
    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnCountResult res = rcnCountLogicalLines(12345, source); // NOLINT
    TEST_ASSERT_FALSE(res.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_UNSUPPORTED_FORMAT, res.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "The input format or programming language is not supported",
        res.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, res.count);
}

void testLogicalLineCountSimpleJavaIsSuccessful(void) {
    char* code =
        "package mytest;\n"
        "public class A {\n"
        "    int m() { int x = 0; return x;}\n"
        "}\n";

    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnCountResult res = rcnCountLogicalLines(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(res.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, res.state.errorCode);
    TEST_ASSERT_NULL(res.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(5, res.count);
}

void testLogicalLineCountWithCarriageReturnAndLineFeed(void) {
    char* code = (
        "package mytest;\r\n"
        "public class A {\r\n"
        "    int m() { int x = 0; return x;}\r\n"
        "}\r\n"
    );
    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnCountResult res = rcnCountLogicalLines(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(res.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, res.state.errorCode);
    TEST_ASSERT_NULL(res.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(5, res.count);
}

void testLogicalLineCountWithEncodedSourceUTF8(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_8);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_JAVA,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(16, result.count);
}

void testLogicalLineCountWithEncodedSourceUTF16LE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_LE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_JAVA,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(16, result.count);
}

void testLogicalLineCountWithEncodedSourceUTF16BE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_BE);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLogicalLines(
        RCN_LANG_JAVA,
        file->content
    );
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(16, result.count);
}

void testLogicalLineCountWithTooLargeTextInputFails(void) {
    RcnSourceText source = {
        .text = "AAAAAA....AAAA",
        .size = 0x000000FFFFFFFFFF
    };
    RcnCountResult result = rcnCountLogicalLines(RCN_LANG_C, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INPUT_TOO_LARGE, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Source input exceeds maximum supported size",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testEvaluateSourceTreeFailsWhenGivenInputWithUnknownLanguage(void) {
    char* code = (
        "clazz Dat [\n"
        "    priblic fn do / / ( prind('What the hell is this language?') )\n"
        "]\n"
    );
    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnResultState result = evaluateSourceTree(
        source,
        12345, // NOLINT
        NULL,
        NULL
    );
    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_UNSUPPORTED_FORMAT, result.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "The input language is not supported",
        result.errorMessage
    );
}

void testMarkLogicalLinesSimpleJavaIsSuccessful(void) {
    char* code =
        "package mytest;\n"
        "public class A {\n"
        "    public int m() { int x = 0; return x;}\n"
        "}\n";

    char* expected =
        "package mytest; // +1 (package declaration)\n"
        "public class A { // +1 (class declaration)\n"
        "    public int m() { int x = 0; return x;} "
        "// +3 (method declaration, local variable declaration, "
        "return statement)\n"
        "}\n";

    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVA,
        source
    );
    TEST_ASSERT_NOT_NULL(marked.text);
    TEST_ASSERT_EQUAL_STRING(expected, marked.text);
    free(marked.text);
}

void testMarkLogicalLinesWithTooLargeTextInputFails(void) {
    RcnSourceText source = {
        .text = "AAAAAA....AAAA",
        .size = 0x000000FFFFFFFFFF
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(RCN_LANG_C, source);
    TEST_ASSERT_NULL(marked.text);
    TEST_ASSERT_EQUAL_INT(0, marked.size);
}

void testMarkLogicalLinesWithNullTextInputFails(void) {
    RcnSourceText source = {
        .text = NULL,
        .size = 1
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVA,
        source
    );
    TEST_ASSERT_NULL(marked.text);
    TEST_ASSERT_EQUAL_INT(0, marked.size);
}

void testMarkLogicalLinesWithNullFilePathInputFails(void) {
    char* path = NULL;
    RcnSourceText marked = rcnMarkLogicalLinesInFile(path);
    TEST_ASSERT_NULL(marked.text);
    TEST_ASSERT_EQUAL_INT(0, marked.size);
}

void testMarkLogicalLinesWithManyHitsOnOneLine(void) {
    char* code = (
        "package somepack; public class A { public int m() { "
        "int x = 0; return x;} }\n"
    );

    char* expected = (
        "package somepack; public class A {"
        " public int m() { int x = 0; return x;} }"
        " // +5 (package declaration, class declaration, method declaration,"
        " local variable declaration, return statement)\n"
    );

    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVA,
        source
    );
    TEST_ASSERT_NOT_NULL(marked.text);
    TEST_ASSERT_EQUAL_STRING(expected, marked.text);
    free(marked.text);
}

void testMarkLogicalLineCountWithCarriageReturnAndLineFeed(void) {
    char* codeWithCrLf = (
        "package mytest;\r\n"
        "public class A {\r\n"
        "    int m() {\r\n"
        "        int x = 0;\r\n"
        "        return x;\r\n"
        "    }\r\n"
        "}\r\n"
    );
    char* expected = (
        "package mytest; // +1 (package declaration)\r\n"
        "public class A { // +1 (class declaration)\r\n"
        "    int m() { // +1 (method declaration)\r\n"
        "        int x = 0; // +1 (local variable declaration)\r\n"
        "        return x; // +1 (return statement)\r\n"
        "    }\r\n"
        "}\r\n"
    );
    RcnSourceText source = {
        .text = codeWithCrLf,
        .size = strlen(codeWithCrLf)
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVA,
        source
    );
    TEST_ASSERT_NOT_NULL(marked.text);
    TEST_ASSERT_EQUAL_STRING(expected, marked.text);
    free(marked.text);
}

void testMarkLogicalLineCountLastLineWithoutLineFeed(void) {
    char* codeWithoutFinalLf = (
        "package mytest;\n"
        "public class A {\n"
        "int m() { int x = 0; return x; } }"
    );
    char* expected = (
        "package mytest; // +1 (package declaration)\n"
        "public class A { // +1 (class declaration)\n"
        "int m() { int x = 0; return x; } }"
        " // +3 (method declaration, local variable declaration,"
        " return statement)"
    );
    RcnSourceText source = {
        .text = codeWithoutFinalLf,
        .size = strlen(codeWithoutFinalLf)
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(
        RCN_LANG_JAVA,
        source
    );
    TEST_ASSERT_NOT_NULL(marked.text);
    TEST_ASSERT_EQUAL_STRING(expected, marked.text);
    free(marked.text);
}

void testLogicalLineCountWithSyntaxErrorFails(void) {
    char* code =
        "public bla class A {\n"
        "  void m( { }\n"
        "}\n";

    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnCountResult res = rcnCountLogicalLines(RCN_LANG_JAVA, source);
    TEST_ASSERT_FALSE(res.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_SYNTAX_ERROR, res.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Syntax error detected in source code",
        res.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, res.count);
}

void testMarkLogicalLinesFailsWhenGivenUTF16Input(void) {
    RcnSourceText result = rcnMarkLogicalLinesInFile(
        TEST_FILE_SOURCE_UTF_16_LE
    );
    TEST_ASSERT_NULL(result.text);
    TEST_ASSERT_EQUAL_INT(0, result.size);
}

void testMarkLogicalLinesWorksWhenGivenUTF8InputWithBOM(void) {
    RcnSourceText result = rcnMarkLogicalLinesInFile(
        TEST_FILE_SOURCE_UTF_8_BOM
    );
    TEST_ASSERT_NOT_NULL(result.text);
    TEST_ASSERT_EQUAL_INT(991, result.size);
    TEST_ASSERT_NOT_NULL(strstr(result.text, "// +1 (class declaration)"));
    TEST_ASSERT_NOT_NULL(strstr(result.text, "// +1 (method declaration)"));
    TEST_ASSERT_NOT_NULL(strstr(result.text, "// +1 (expression statement)"));
    rcnFreeSourceText(&result);
}

void testMarkLogicalLinesFailsWhenGivenInputWithUnknownLanguage(void) {
    char* code = (
        "class Data {\n"
        "    int bla() { }\n"
        "}\n"
    );
    RcnSourceText source = {
        .text = code,
        .size = strlen(code)
    };
    RcnSourceText marked = rcnMarkLogicalLinesInSourceText(
        12345, // NOLINT
        source
    );
    TEST_ASSERT_NULL(marked.text);
    TEST_ASSERT_EQUAL_INT(0, marked.size);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testLogicalLineCountWithInvalidInputReturnsNull);
    RUN_TEST(testLogicalCountMarkWithInvalidInputReturnsNull);
    RUN_TEST(testLogicalCountFailsWhenGivenInputWithUnknownLanguage);
    RUN_TEST(testLogicalLineCountSimpleJavaIsSuccessful);
    RUN_TEST(testLogicalLineCountWithCarriageReturnAndLineFeed);
    RUN_TEST(testLogicalLineCountWithEncodedSourceUTF8);
    RUN_TEST(testLogicalLineCountWithEncodedSourceUTF16LE);
    RUN_TEST(testLogicalLineCountWithEncodedSourceUTF16BE);
    RUN_TEST(testLogicalLineCountWithTooLargeTextInputFails);
    RUN_TEST(testEvaluateSourceTreeFailsWhenGivenInputWithUnknownLanguage);
    RUN_TEST(testLogicalLineCountWithSyntaxErrorFails);
    RUN_TEST(testMarkLogicalLinesSimpleJavaIsSuccessful);
    RUN_TEST(testMarkLogicalLinesWithTooLargeTextInputFails);
    RUN_TEST(testMarkLogicalLinesWithNullTextInputFails);
    RUN_TEST(testMarkLogicalLinesWithNullFilePathInputFails);
    RUN_TEST(testMarkLogicalLinesWithManyHitsOnOneLine);
    RUN_TEST(testMarkLogicalLineCountWithCarriageReturnAndLineFeed);
    RUN_TEST(testMarkLogicalLineCountLastLineWithoutLineFeed);
    RUN_TEST(testMarkLogicalLinesFailsWhenGivenUTF16Input);
    RUN_TEST(testMarkLogicalLinesWorksWhenGivenUTF8InputWithBOM);
    RUN_TEST(testMarkLogicalLinesFailsWhenGivenInputWithUnknownLanguage);
    return UNITY_END();
}
