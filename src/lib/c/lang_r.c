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

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "reckon_export.h"
#include "evaluation.h"

RECKON_NO_EXPORT const TSLanguage* tree_sitter_r(void);

/**
 * These are the symbol identifiers as defined by the R language parser
 * of tree-sitter. We have only copied the symbol identifiers that we are
 * interested in evaluating or counting. Others do not contribute to the weight
 * of a node in the AST.
 */
enum SymbolIdentifiersR {
    sym_return = 52,
    sym_next = 53,
    sym_break = 54,
    sym_program = 81,
    sym_function_definition = 82,
    sym_if_statement = 88,
    sym_for_statement = 89,
    sym_while_statement = 90,
    sym_repeat_statement = 91,
    sym_braced_expression = 92,
    sym_parenthesized_expression = 93,
    sym_call = 94,
    sym_unary_operator = 104,
    sym_binary_operator = 105,
};

TSParser* createParserR(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_r())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

/**
 * Checks whether the given node's immediate parent is the program node
 * or a braced expression, i.e. the node is at statement level.
 */
static bool isAtStatementLevel(TSNode node) {
    TSNode parent = ts_node_parent(node);
    if (ts_node_is_null(parent)) {
        return false;
    }
    TSSymbol sym = ts_node_grammar_symbol(parent);
    return sym == sym_program || sym == sym_braced_expression;
}

static RcnCount evaluateNodeWeightRImpl(TSNode node, NodeEvalTrace* trace) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    switch (sym) {
        case sym_binary_operator:
        case sym_call:
        case sym_unary_operator:
        case sym_function_definition:
        case sym_parenthesized_expression:
        case sym_if_statement:
        case sym_for_statement:
        case sym_while_statement:
        case sym_repeat_statement:
        case sym_next:
        case sym_break:
        case sym_return:
            if (isAtStatementLevel(node)) {
                weight += 1;
            }
            break;
        default:
            break;
    }
    return weight;
}

void evaluateNodeR(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightRImpl(node, trace);
    trace->idx++;
}
