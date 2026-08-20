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

#include <stdbool.h>

#include "unity.h"

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "evaluation.h"

void setUp(void) { }

void tearDown(void) { }

// NOLINTBEGIN(readability-magic-numbers)

void testCreateParserForJava(void) {
    RcnTextFormat language = RCN_LANG_JAVA;
    TSParser* parser = createParser(language);
    TEST_ASSERT_NOT_NULL(parser);
    ts_parser_delete(parser);
}

void testCreateEvaluationFunctionForJava(void) {
    RcnTextFormat language = RCN_LANG_JAVA;
    NodeVisitor evaluator = createEvaluationFunction(language);
    TEST_ASSERT_NOT_NULL(evaluator);
}

void testGetInlineSourceCommentStringForJava(void) {
    RcnTextFormat language = RCN_LANG_JAVA;
    Span string = getInlineSourceCommentString(language);
    TEST_ASSERT_NOT_NULL(string.ptr);
    TEST_ASSERT_EQUAL_STRING("//", string.ptr);
}

void testGetTextFormatLabelForJava(void) {
    const char* label = rcnGetTextFormatLabel(RCN_LANG_JAVA);
    TEST_ASSERT_NOT_NULL(label);
    TEST_ASSERT_EQUAL_STRING("Java", label);
}

void testFormatCapabilityQueriesForCMake(void) {
    bool hasLlc = rcnIsLlcCountingSupported(RCN_TEXT_CMAKE);
    bool hasLoc = rcnIsLocCountingSupported(RCN_TEXT_CMAKE);
    TEST_ASSERT_FALSE(hasLlc);
    TEST_ASSERT_TRUE(hasLoc);
}

void testFormatCapabilityQueriesForJava(void) {
    bool hasLlc = rcnIsLlcCountingSupported(RCN_LANG_JAVA);
    bool hasLoc = rcnIsLocCountingSupported(RCN_LANG_JAVA);
    TEST_ASSERT_TRUE(hasLlc);
    TEST_ASSERT_TRUE(hasLoc);
}

void testAllFormatEnumeratorsHaveLabels(void) {
    for (RcnTextFormat frmt = 0; frmt < RECKON_NUM_SUPPORTED_FORMATS; ++frmt) {
        const char* label = rcnGetTextFormatLabel(frmt);
        TEST_ASSERT_NOT_NULL(label);
    }
}

void testCreateParserForUnknownLanguageReturnsNull(void) {
    TSParser* parser = createParser(12345); // NOLINT
    TEST_ASSERT_NULL(parser);
}

void testCreateEvaluationFunctionForUnknownLanguageReturnsNull(void) {
    NodeVisitor evaluator = createEvaluationFunction(12345); // NOLINT
    TEST_ASSERT_NULL(evaluator);
}

void testGetInlineSourceCommentStrForUnknownLangReturnsDefaultValue(void) {
    Span string = getInlineSourceCommentString(12345); // NOLINT
    TEST_ASSERT_NOT_NULL(string.ptr);
    TEST_ASSERT_EQUAL_STRING("//", string.ptr);
}

void testGetTextFormatLabelForUnknownFormatReturnsNull(void) {
    const char* label = rcnGetTextFormatLabel(12345); // NOLINT
    TEST_ASSERT_NULL(label);
}

void testFormatCapabilityQueriesForUnknownFormatReturnFalse(void) {
    bool hasLlc = rcnIsLlcCountingSupported(12345); // NOLINT
    bool hasLoc = rcnIsLocCountingSupported(12345); // NOLINT
    TEST_ASSERT_FALSE(hasLlc);
    TEST_ASSERT_FALSE(hasLoc);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCreateParserForJava);
    RUN_TEST(testCreateEvaluationFunctionForJava);
    RUN_TEST(testGetInlineSourceCommentStringForJava);
    RUN_TEST(testGetTextFormatLabelForJava);
    RUN_TEST(testFormatCapabilityQueriesForCMake);
    RUN_TEST(testFormatCapabilityQueriesForJava);
    RUN_TEST(testAllFormatEnumeratorsHaveLabels);
    RUN_TEST(testCreateParserForUnknownLanguageReturnsNull);
    RUN_TEST(testCreateEvaluationFunctionForUnknownLanguageReturnsNull);
    RUN_TEST(testGetInlineSourceCommentStrForUnknownLangReturnsDefaultValue);
    RUN_TEST(testGetTextFormatLabelForUnknownFormatReturnsNull);
    RUN_TEST(testFormatCapabilityQueriesForUnknownFormatReturnFalse);
    return UNITY_END();
}
