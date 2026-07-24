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
#include <stddef.h>
#include <string.h>

#include "unity.h"

#include "reckon/reckon.h"
#include "fileio.h"

#define TEST_DIR_ENC RECKON_TEST_PATH_RES_BASE "/encodings"
#define TEST_FILE_SOURCE_UTF_8 TEST_DIR_ENC "/Source_UTF_8.java"
#define TEST_FILE_SOURCE_UTF_8_BOM TEST_DIR_ENC "/Source_UTF_8_with_BOM.java"
#define TEST_FILE_SOURCE_UTF_16_LE TEST_DIR_ENC "/Source_UTF_16LE.java"
#define TEST_FILE_SOURCE_UTF_16_BE TEST_DIR_ENC "/Source_UTF_16BE.java"
#define TEST_FILE_SOURCE_UTF_16_LE_BC TEST_DIR_ENC "/MiscSource_UTF_16LE_WithBlockComments.java"

void setUp(void) { }

void tearDown(void) { }

// NOLINTBEGIN(readability-magic-numbers)

/**
 * Helper function to convert ASCII text to UTF-16LE encoded bytes.
 * Adds a UTF-16LE BOM at the beginning and zero bytes after each character.
 * The caller is responsible for freeing the returned RcnSourceText.text.
 */
static RcnSourceText convertToUTF16LE(const char* text) {
    const size_t length = strlen(text);
    const size_t utf16Size = 2 + (length * 2);
    char* buffer = (char*) malloc(utf16Size);

    buffer[0] = (char) 0xff;
    buffer[1] = (char) 0xfe;

    for (size_t i = 0; i < length; ++i) {
        buffer[2 + (i * 2)] = text[i];
        buffer[2 + (i * 2) + 1] = 0x00;
    }
    return (RcnSourceText){
        .text = buffer,
        .size = utf16Size
    };
}

void testCodeLineCountIsCorrect(void) {
    char* text =
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
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_8_BOM);
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
    char* text =
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

void testCodeLineCountWithNonProgrammingLanguageReturnsError(void) {
    char *markDown =
        "# A Big Title\n"
        "\n"
        "### A smaller title\n"
        "\n"
        "* Something\n"
        "* Some **Text**\n"
        "* Other\n"
        "\n"
        "\n";

    RcnSourceText source = {
        .text = markDown,
        .size = strlen(markDown)
    };

    RcnCountResult result = rcnCountLinesOfCode(RCN_TEXT_MARKDOWN, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_UNSUPPORTED_FORMAT, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "The LOC count input format or programming language is not supported",
        result.state.errorMessage)
    ;
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCodeLineCountWithTooLargeTextInputFails(void) {
    RcnSourceText source = {
        .text = "AAAAAA....AAAA",
        .size = 0x000000FFFFFFFFFF
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_FALSE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_INPUT_TOO_LARGE, result.state.errorCode);
    TEST_ASSERT_EQUAL_STRING(
        "Input exceeds maximum supported size",
        result.state.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCodeLineCountWithBlockCommentsEncodedinUTF16LE(void) {
    RcnSourceFile* file = newSourceFile(TEST_FILE_SOURCE_UTF_16_LE_BC);
    readSourceFileContent(file);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, file->content);
    freeSourceFile(file);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(19, result.count);
}

void testCodeLineCountWithOnlyCommentsReturnsZero(void) {
    char* text =
        "// This is a comment\n"
        "// Another comment\n"
        "/* Block comment line 1\n"
        "   Block comment line 2\n"
        "   Block comment line 3 */\n"
        "// Final comment\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCodeLineCountWithOnlyWhitespaceReturnsZero(void) {
    char* text =
        "   \n"
        "\t\t\n"
        "  \t  \n"
        "\n"
        "     \n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
}

void testCodeLineCountWithCodeFollowedByLineComment(void) {
    char* text =
        "int x = 5;// Initialize variable\n"
        "int y = 10; // Another variable\n"
        "return x + y;    // Return sum\n  ";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
}

void testCodeLineCountWithBlockCommentOnSameLineAsCode(void) {
    char* text =
        "int x; /* comment */ int y;\n"
        "x=5;/*initialize*/y=10;\n"
        "return /* inline */ x + y;\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
}

void testCodeLineCountWithUnclosedBlockComment(void) {
    char* text =
        "int x = 5;\n"
        "int y = 10;\n"
        "/* This block comment is never closed\n"
        "int z = 15;\n"
        "return x + y + z;\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(2, result.count);
}

void testCodeLineCountWithMultipleBlockCommentsOnOneLine(void) {
    char* text =
        "/* comment a */ int x = 5; /* comment b */\n"
        "/* c */ int y /*d*/ = /*   e   */ 10;\n"
        "return x + y;\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
}

void testCodeLineCountWithOnlyBlockCommentReturnsZero(void) {
    char* text =
        "/* This is a single-line block comment */\n"
        "   /* Another one with leading spaces */  \n"
        "int x = 5;\n"
        "\t/* And one with a tab */\n";

    RcnSourceText source = {
        .text = text,
        .size = strlen(text)
    };
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(1, result.count);
}

void testCodeLineCountWithOnlyCommentsReturnsZeroUTF16LE(void) {
    char* text =
        "// This is a comment\n"
        "// Another comment\n"
        "/* Block comment line 1\n"
        "   Block comment line 2\n"
        "   Block comment line 3 */\n"
        "// Final comment\n";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
    rcnFreeSourceText(&source);
}

void testCodeLineCountWithOnlyWhitespaceReturnsZeroUTF16LE(void) {
    char* text =
        "   \n"
        "\t\t\n"
        "  \t  \n"
        "\n"
        "     \n";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, result.count);
    rcnFreeSourceText(&source);
}

void testCodeLineCountWithCodeFollowedByLineCommentUTF16LE(void) {
    char* text =
        "int x = 5;// Initialize variable\n"
        "int y = 10; // Another variable\n"
        "return x + y;    // Return sum\n  ";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
    rcnFreeSourceText(&source);
}

void testCodeLineCountWithBlockCommentOnSameLineAsCodeUTF16LE(void) {
    char* text =
        "int x; /* comment */ int y;\n"
        "x=5;/*initialize*/y=10;\n"
        "return /* inline */ x + y;\n";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
    rcnFreeSourceText(&source);
}

void testCodeLineCountWithUnclosedBlockCommentUTF16LE(void) {
    char* text =
        "int x = 5;\n"
        "int y = 10;\n"
        "/* This block comment is never closed\n"
        "int z = 15;\n"
        "return x + y + z;\n";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(2, result.count);
    rcnFreeSourceText(&source);
}

void testCodeLineCountWithMultipleBlockCommentsOnOneLineUTF16LE(void) {
    char* text =
        "/* comment a */ int x = 5; /* comment b */\n"
        "/* c */ int y /*d*/ = /*   e   */ 10;\n"
        "return x + y;\n";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(3, result.count);
    rcnFreeSourceText(&source);
}

void testCodeLineCountWithOnlyBlockCommentReturnsZeroUTF16LE(void) {
    char* text =
        "/* This is a single-line block comment */\n"
        "   /* Another one with leading spaces */  \n"
        "int x = 5;\n"
        "\t/* And one with a tab */\n";

    RcnSourceText source = convertToUTF16LE(text);
    RcnCountResult result = rcnCountLinesOfCode(RCN_LANG_JAVA, source);
    TEST_ASSERT_TRUE(result.state.ok);
    TEST_ASSERT_EQUAL_INT(RCN_ERR_NONE, result.state.errorCode);
    TEST_ASSERT_NULL(result.state.errorMessage);
    TEST_ASSERT_EQUAL_INT(1, result.count);
    rcnFreeSourceText(&source);
}

// NOLINTEND(readability-magic-numbers)

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
    RUN_TEST(testCodeLineCountWithNonProgrammingLanguageReturnsError);
    RUN_TEST(testCodeLineCountWithTooLargeTextInputFails);
    RUN_TEST(testCodeLineCountWithBlockCommentsEncodedinUTF16LE);
    RUN_TEST(testCodeLineCountWithOnlyCommentsReturnsZero);
    RUN_TEST(testCodeLineCountWithOnlyWhitespaceReturnsZero);
    RUN_TEST(testCodeLineCountWithCodeFollowedByLineComment);
    RUN_TEST(testCodeLineCountWithBlockCommentOnSameLineAsCode);
    RUN_TEST(testCodeLineCountWithUnclosedBlockComment);
    RUN_TEST(testCodeLineCountWithMultipleBlockCommentsOnOneLine);
    RUN_TEST(testCodeLineCountWithOnlyBlockCommentReturnsZero);
    RUN_TEST(testCodeLineCountWithOnlyCommentsReturnsZeroUTF16LE);
    RUN_TEST(testCodeLineCountWithOnlyWhitespaceReturnsZeroUTF16LE);
    RUN_TEST(testCodeLineCountWithCodeFollowedByLineCommentUTF16LE);
    RUN_TEST(testCodeLineCountWithBlockCommentOnSameLineAsCodeUTF16LE);
    RUN_TEST(testCodeLineCountWithUnclosedBlockCommentUTF16LE);
    RUN_TEST(testCodeLineCountWithMultipleBlockCommentsOnOneLineUTF16LE);
    RUN_TEST(testCodeLineCountWithOnlyBlockCommentReturnsZeroUTF16LE);
    return UNITY_END();
}
