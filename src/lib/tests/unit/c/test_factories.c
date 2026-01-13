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
    const char* string = getInlineSourceCommentString(language);
    TEST_ASSERT_NOT_NULL(string);
    TEST_ASSERT_EQUAL_STRING("//", string);
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
    const char* string = getInlineSourceCommentString(12345); // NOLINT
    TEST_ASSERT_NOT_NULL(string);
    TEST_ASSERT_EQUAL_STRING("//", string);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testCreateParserForJava);
    RUN_TEST(testCreateEvaluationFunctionForJava);
    RUN_TEST(testGetInlineSourceCommentStringForJava);
    RUN_TEST(testCreateParserForUnknownLanguageReturnsNull);
    RUN_TEST(testCreateEvaluationFunctionForUnknownLanguageReturnsNull);
    RUN_TEST(testGetInlineSourceCommentStrForUnknownLangReturnsDefaultValue);
    return UNITY_END();
}
