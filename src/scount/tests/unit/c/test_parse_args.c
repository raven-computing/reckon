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

#include "unity.h"

#include "reckon/reckon.h"
#include "scount.h"

void setUp(void) { }

void tearDown(void) { }

void testNoArgsSetsMessageNoInputAndInvalid(void) {
    char* argv[] = { "scount" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_NULL(args.inputPath);
    TEST_ASSERT_FALSE(args.annotateCounts);
    TEST_ASSERT_FALSE(args.linesOnly);
    TEST_ASSERT_FALSE(args.verbose);
    TEST_ASSERT_FALSE(args.help);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING("No input path specified.", args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testSingleInputSetsInputPathAndValid(void) {
    char* argv[] = { "scount", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_FALSE(args.annotateCounts);
    TEST_ASSERT_FALSE(args.linesOnly);
    TEST_ASSERT_FALSE(args.verbose);
    TEST_ASSERT_FALSE(args.help);
    TEST_ASSERT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testAnnotateAndVerboseFlagsSetBooleans(void) {
    char* argv[] = { "scount", "--annotate-counts", "--verbose", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.annotateCounts);
    TEST_ASSERT_FALSE(args.linesOnly);
    TEST_ASSERT_TRUE(args.verbose);
    TEST_ASSERT_FALSE(args.help);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testHelpFlagSetsHelpTrueAndMessageNoInput(void) {
    char* argv[] = { "scount", "--help" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_TRUE(args.help);
    TEST_ASSERT_NULL(args.inputPath);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING("No input path specified.", args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testHelpAliasQuestionMarkSetsHelpTrue(void) {
    char* argv[] = { "scount", "-?" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_TRUE(args.help);
    TEST_ASSERT_NULL(args.inputPath);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING("No input path specified.", args.errorMessage);
}

void testVersionAliasHashSetsVersionTrue(void) {
    char* argv[] = { "scount", "-#" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_TRUE(args.version);
    TEST_ASSERT_TRUE(args.versionShort);
    TEST_ASSERT_NULL(args.inputPath);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING("No input path specified.", args.errorMessage);
}

void testVersionFullDoesNotSetVersionShortTrue(void) {
    char* argv[] = { "scount", "--version" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_TRUE(args.version);
    TEST_ASSERT_FALSE(args.versionShort);
    TEST_ASSERT_NULL(args.inputPath);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING("No input path specified.", args.errorMessage);
}

void testUnknownOptionBeforeInputSetsIndexUnknownAndMessage(void) {
    char* argv[] = { "scount", "-x", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_EQUAL_INT(1, args.indexUnknown);
    TEST_ASSERT_NULL(args.inputPath);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING("No input path specified.", args.errorMessage);
}

void testUnknownOptionAfterInputSetsIndexUnknownAndKeepsInput(void) {
    char* argv[] = { "scount", "File.java", "--bla" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_EQUAL_INT(2, args.indexUnknown);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testMultipleInputsSetsMessageMultiple(void) {
    char* argv[] = { "scount", "File1.java", "File2.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_EQUAL_STRING("File1.java", args.inputPath);
    TEST_ASSERT_NOT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_STRING(
        "Multiple input paths specified.", args.errorMessage
    );
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testFlagsAndInputOrderMixed(void) {
    char* argv[] = { "scount", "--verbose", "File.java", "--annotate-counts" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.verbose);
    TEST_ASSERT_TRUE(args.annotateCounts);
    TEST_ASSERT_FALSE(args.linesOnly);
    TEST_ASSERT_FALSE(args.help);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testHelpWithInputSetsHelpAndInput(void) {
    char* argv[] = { "scount", "--help", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.help);
    TEST_ASSERT_FALSE(args.linesOnly);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    // message remains NULL because input is present
    TEST_ASSERT_NULL(args.errorMessage);
}

void testLinesLongOptionSetsLinesOnlyTrue(void) {
    char* argv[] = { "scount", "--lines", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.linesOnly);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testLinesShortOptionSetsLinesOnlyTrue(void) {
    char* argv[] = { "scount", "-l", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.linesOnly);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testTotalOptionSetsTotalsOnlyTrue(void) {
    char* argv[] = { "scount", "--total", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.totalsOnly);
    TEST_ASSERT_FALSE(args.linesOnly);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testStrictOptionSetsStrictTrue(void) {
    char* argv[] = { "scount", "--strict", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.strict);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testNoParallelizationOptionSetsFlagTrue(void) {
    char* argv[] = { "scount", "--disable-parallelization", "File.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.disableParallelization);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testIncludeFormatOptionParsesCommaSeparatedCaseInsensitive(void) {
    char* argv[] = {
        "scount",
        "--include-format",
        "java,C,txt",
        "File.java"
    };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    const uint32_t expectedMask = (
        (uint32_t) RECKON_MK_FRMT_OPT(RCN_LANG_JAVA)
        | (uint32_t) RECKON_MK_FRMT_OPT(RCN_LANG_C)
        | (uint32_t) RECKON_MK_FRMT_OPT(RCN_TEXT_UNFORMATTED)
    );

    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.hasIncludeFormatMask);
    TEST_ASSERT_EQUAL_HEX32(expectedMask, args.includeFormatMask);
    TEST_ASSERT_FALSE(args.hasExcludeFormatMask);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testIncludeFormatOptionHandlesSpacesInFormatArgument(void) {
    char* argv[] = {
        "scount",
        "--include-format",
        "  java  ,   C ,    txt    ",
        "File.java"
    };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    const uint32_t expectedMask = (
        (uint32_t) RECKON_MK_FRMT_OPT(RCN_LANG_JAVA)
        | (uint32_t) RECKON_MK_FRMT_OPT(RCN_LANG_C)
        | (uint32_t) RECKON_MK_FRMT_OPT(RCN_TEXT_UNFORMATTED)
    );

    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.hasIncludeFormatMask);
    TEST_ASSERT_EQUAL_HEX32(expectedMask, args.includeFormatMask);
    TEST_ASSERT_FALSE(args.hasExcludeFormatMask);
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testFormatOptionsCanBeSpecifiedMultipleTimes(void) {
    char* argv[] = {
        "scount",
        "--include-format",
        "java",
        "--include-format",
        "txt",
        "--exclude-format",
        "C",
        "--exclude-format",
        "md",
        "File.java"
    };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    const uint32_t expectedIncludeMask = (
        (uint32_t) RECKON_MK_FRMT_OPT(RCN_LANG_JAVA)
        | (uint32_t) RECKON_MK_FRMT_OPT(RCN_TEXT_UNFORMATTED)
    );
    const uint32_t expectedExcludeMask = (
        (uint32_t) RECKON_MK_FRMT_OPT(RCN_LANG_C)
        | (uint32_t) RECKON_MK_FRMT_OPT(RCN_TEXT_MARKDOWN)
    );

    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_TRUE(args.hasIncludeFormatMask);
    TEST_ASSERT_TRUE(args.hasExcludeFormatMask);
    TEST_ASSERT_EQUAL_HEX32(expectedIncludeMask, args.includeFormatMask);
    TEST_ASSERT_EQUAL_HEX32(expectedExcludeMask, args.excludeFormatMask);
    TEST_ASSERT_NULL(args.errorMessage);
}

void testIncludeFormatOptionMissingArgumentSetsError(void) {
    char* argv[] = { "scount", "--include-format" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);

    TEST_ASSERT_FALSE(isValid);
    TEST_ASSERT_EQUAL_STRING(
        "Missing argument for option '--include-format'.",
        args.errorMessage
    );
}

void testExcludeFormatOptionWithInvalidSpecifierIsIgnored(void) {
    char* argv[] = {
        "scount",
        "--exclude-format",
        "this-is-not-a-format",
        "File.java"
    };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    
    TEST_ASSERT_NOT_NULL(args.invalidExcludeFormatSpecifier);
    TEST_ASSERT_EQUAL_STRING("this-is-not-a-format", args.invalidExcludeFormatSpecifier);
    TEST_ASSERT_FALSE(args.hasExcludeFormatMask);
    TEST_ASSERT_NULL(args.errorMessage);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
}

void testAllSupportedIncludeExcludeFormatSpecifiers(void) {
    // Hint: If this test fails, it likely means that a new text format was
    // introduced and you forgot to add the corresponding handling for it in
    // the parseFormatSpecifier() function.
    for (RcnTextFormat frmt = 0; frmt < RECKON_NUM_SUPPORTED_FORMATS; ++frmt) {
        const char* label = rcnGetTextFormatLabel(frmt);
        char* argv[] = {
            "scount", "--include-format", (char*) label, "unusedInput"
        };
        int argc = (int)(sizeof(argv) / sizeof(argv[0]));
        AppArgs args = parseArgs(argc, argv);

        TEST_ASSERT_NULL(args.invalidIncludeFormatSpecifier);
        TEST_ASSERT_TRUE(args.hasIncludeFormatMask);
        uint32_t expectedMask = (uint32_t) RECKON_MK_FRMT_OPT(frmt);
        TEST_ASSERT_EQUAL(expectedMask, args.includeFormatMask);
        TEST_ASSERT_NULL(args.invalidExcludeFormatSpecifier);
        TEST_ASSERT_FALSE(args.hasExcludeFormatMask);
        TEST_ASSERT_EQUAL(0, args.excludeFormatMask);
        TEST_ASSERT_NULL(args.errorMessage);
        bool isValid = isInputValid(args);
        TEST_ASSERT_TRUE(isValid);

        argv[1] = "--exclude-format";
        argc = (int)(sizeof(argv) / sizeof(argv[0]));
        args = parseArgs(argc, argv);
        TEST_ASSERT_NULL(args.invalidIncludeFormatSpecifier);
        TEST_ASSERT_FALSE(args.hasIncludeFormatMask);
        expectedMask = (uint32_t) RECKON_MK_FRMT_OPT(frmt);
        TEST_ASSERT_EQUAL(0, args.includeFormatMask);
        TEST_ASSERT_NULL(args.invalidExcludeFormatSpecifier);
        TEST_ASSERT_TRUE(args.hasExcludeFormatMask);
        TEST_ASSERT_EQUAL(expectedMask, args.excludeFormatMask);
        TEST_ASSERT_NULL(args.errorMessage);
        isValid = isInputValid(args);
        TEST_ASSERT_TRUE(isValid);
    }
}

void testStdinPathDashSetsInputPathAndValid(void) {
    char* argv[] = { "scount", "-" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_EQUAL_STRING("", args.inputPath);
    TEST_ASSERT_TRUE(args.readFromStdin);
    TEST_ASSERT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

void testStdinPathDashDotFileExtensionSetsInputPathAndValid(void) {
    char* argv[] = { "scount", "-.java" };
    int argc = (int)(sizeof(argv) / sizeof(argv[0]));
    AppArgs args = parseArgs(argc, argv);
    bool isValid = isInputValid(args);
    TEST_ASSERT_TRUE(isValid);
    TEST_ASSERT_EQUAL_STRING(".java", args.inputPath);
    TEST_ASSERT_TRUE(args.readFromStdin);
    TEST_ASSERT_NULL(args.errorMessage);
    TEST_ASSERT_EQUAL_INT(0, args.indexUnknown);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testNoArgsSetsMessageNoInputAndInvalid);
    RUN_TEST(testSingleInputSetsInputPathAndValid);
    RUN_TEST(testAnnotateAndVerboseFlagsSetBooleans);
    RUN_TEST(testHelpFlagSetsHelpTrueAndMessageNoInput);
    RUN_TEST(testHelpAliasQuestionMarkSetsHelpTrue);
    RUN_TEST(testVersionAliasHashSetsVersionTrue);
    RUN_TEST(testVersionFullDoesNotSetVersionShortTrue);
    RUN_TEST(testUnknownOptionBeforeInputSetsIndexUnknownAndMessage);
    RUN_TEST(testUnknownOptionAfterInputSetsIndexUnknownAndKeepsInput);
    RUN_TEST(testMultipleInputsSetsMessageMultiple);
    RUN_TEST(testFlagsAndInputOrderMixed);
    RUN_TEST(testHelpWithInputSetsHelpAndInput);
    RUN_TEST(testLinesLongOptionSetsLinesOnlyTrue);
    RUN_TEST(testLinesShortOptionSetsLinesOnlyTrue);
    RUN_TEST(testTotalOptionSetsTotalsOnlyTrue);
    RUN_TEST(testStrictOptionSetsStrictTrue);
    RUN_TEST(testNoParallelizationOptionSetsFlagTrue);
    RUN_TEST(testIncludeFormatOptionParsesCommaSeparatedCaseInsensitive);
    RUN_TEST(testIncludeFormatOptionHandlesSpacesInFormatArgument);
    RUN_TEST(testFormatOptionsCanBeSpecifiedMultipleTimes);
    RUN_TEST(testIncludeFormatOptionMissingArgumentSetsError);
    RUN_TEST(testExcludeFormatOptionWithInvalidSpecifierIsIgnored);
    RUN_TEST(testAllSupportedIncludeExcludeFormatSpecifiers);
    RUN_TEST(testStdinPathDashSetsInputPathAndValid);
    RUN_TEST(testStdinPathDashDotFileExtensionSetsInputPathAndValid);
    return UNITY_END();
}
