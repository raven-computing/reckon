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

#include <string.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "reckon_export.h"
#include "evaluation.h"

RECKON_NO_EXPORT const TSLanguage* tree_sitter_python(void);

static const char* S_EXPR_PY_DOCSTRING = (
"(expression_statement (string (string_start) (string_content) (string_end)))"
);

/**
 * These are the symbol identifiers as defined by the Python language parser
 * of tree-sitter. We have only copied the symbol identifiers that we are
 * interested in evaluating or counting. Others do not contribute to the weight
 * of a node in the AST.
 */
enum SymbolIdentifiersPython {
  sym_import_statement = 111,
  sym_future_import_statement = 114,
  sym_import_from_statement = 115,
  sym_print_statement = 119,
  sym_assert_statement = 121,
  sym_expression_statement = 122,
  sym_return_statement = 125,
  sym_delete_statement = 126,
  sym_raise_statement = 127,
  sym_pass_statement = 128,
  sym_break_statement = 129,
  sym_continue_statement = 130,
  sym_if_statement = 131,
  sym_elif_clause = 132,
  sym_else_clause = 133,
  sym_match_statement = 134,
  sym_case_clause = 136,
  sym_for_statement = 137,
  sym_while_statement = 138,
  sym_try_statement = 139,
  sym_except_clause = 140,
  sym_finally_clause = 141,
  sym_with_statement = 142,
  sym_function_definition = 145,
  sym_global_statement = 150,
  sym_nonlocal_statement = 151,
  sym_exec_statement = 152,
  sym_type_alias_statement = 153,
  sym_class_definition = 154,
  sym_decorator = 159,
};

TSParser* createParserPython(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_python())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

static RcnCount evaluateNodeWeightPythonImpl(
    TSNode node,
    NodeEvalTrace* trace
) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    switch (sym) {
        case sym_expression_statement: {
            char* sExpression = ts_node_string(node);
            if (sExpression) {
                if (strcmp(sExpression, S_EXPR_PY_DOCSTRING) == 0) {
                    return weight;
                }
                free(sExpression);
            }
            FALLTHROUGH;
        }
        case sym_import_statement:
        case sym_future_import_statement:
        case sym_import_from_statement:
        case sym_print_statement:
        case sym_assert_statement:
        case sym_return_statement:
        case sym_delete_statement:
        case sym_raise_statement:
        case sym_pass_statement:
        case sym_break_statement:
        case sym_continue_statement:
        case sym_if_statement:
        case sym_elif_clause:
        case sym_else_clause:
        case sym_match_statement:
        case sym_case_clause:
        case sym_for_statement:
        case sym_while_statement:
        case sym_try_statement:
        case sym_except_clause:
        case sym_finally_clause:
        case sym_with_statement:
        case sym_function_definition:
        case sym_global_statement:
        case sym_nonlocal_statement:
        case sym_exec_statement:
        case sym_type_alias_statement:
        case sym_class_definition:
        case sym_decorator:
            weight += 1;
        default:
            break;
    }
    return weight;
}

void evaluateNodePython(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightPythonImpl(node, trace);
    trace->idx++;
}
