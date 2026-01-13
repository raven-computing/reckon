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
    TEST_ASSERT_EQUAL_STRING("File.java", args.inputPath);
    // message remains NULL because input is present
    TEST_ASSERT_NULL(args.errorMessage);
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
    return UNITY_END();
}
