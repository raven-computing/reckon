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

RECKON_NO_EXPORT const TSLanguage* tree_sitter_javascript(void);

/**
 * These are the symbol identifiers as defined by the JavaScript language
 * parser of tree-sitter. We have only copied the symbol identifiers that we
 * are interested in evaluating or counting. Others do not contribute to the
 * weight of a node in the AST.
 */
enum SymbolIdentifiersJavaScript {
    sym_hash_bang_line = 2,
    sym_export_statement = 135,
    sym_export_clause = 137,
    sym_declaration = 140,
    sym_import_statement = 142,
    sym_import_clause = 143,
    sym_namespace_import = 145,
    sym_statement = 149,
    sym_expression_statement = 150,
    sym_variable_declaration = 151,
    sym_lexical_declaration = 152,
    sym_using_declaration = 153,
    sym_variable_declarator = 154,
    sym_statement_block = 155,
    sym_else_clause = 156,
    sym_if_statement = 157,
    sym_switch_statement = 158,
    sym_for_statement = 159,
    sym_for_in_statement = 160,
    sym__for_header = 161,
    sym_while_statement = 162,
    sym_do_statement = 163,
    sym_try_statement = 164,
    sym_with_statement = 165,
    sym_break_statement = 166,
    sym_continue_statement = 167,
    sym_debugger_statement = 168,
    sym_return_statement = 169,
    sym_throw_statement = 170,
    sym_labeled_statement = 172,
    sym_switch_case = 174,
    sym_switch_default = 175,
    sym_catch_clause = 176,
    sym_finally_clause = 177,
    sym_parenthesized_expression = 178,
    sym_expression = 179,
    sym_primary_expression = 180,
    sym_yield_expression = 181,
    sym_jsx_expression = 189,
    sym_class_declaration = 198,
    sym_function_expression = 200,
    sym_function_declaration = 201,
    sym_generator_function_declaration = 203,
    sym_arrow_function = 204,
    sym_call_expression = 205,
    sym_new_expression = 206,
    sym_await_expression = 207,
    sym_member_expression = 208,
    sym_assignment_expression = 210,
    sym_augmented_assignment_expression = 212,
    sym_ternary_expression = 216,
    sym_binary_expression = 217,
    sym_unary_expression = 218,
    sym_update_expression = 219,
    sym_sequence_expression = 220,
    sym_decorator = 227,
    sym_decorator_member_expression = 228,
    sym_decorator_call_expression = 229,
    sym_field_definition = 231,
    sym_class_static_block = 233,
    sym_method_definition = 236,
};

TSParser* createParserJavaScript(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_javascript())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

static bool parentHasSymbol(TSNode node, TSSymbol symbol) {
    TSNode parent = ts_node_parent(node);
    return !ts_node_is_null(parent)
        && ts_node_grammar_symbol(parent) == symbol;
}

static bool isJsExportedDeclaration(TSNode node) {
    return parentHasSymbol(node, sym_export_statement);
}

static bool isJsForInitializerDeclaration(TSNode node) {
    return parentHasSymbol(node, sym_for_statement);
}

static bool isJsLabeledBlock(TSNode node) {
    // labeled_statement
    // identifier
    // :
    // statement_block
    // {

    node = ts_node_next_sibling(ts_node_next_sibling(ts_node_child(node, 0)));
    if (!ts_node_is_null(node)) {
        TSSymbol symbol = ts_node_grammar_symbol(node);
        return sym_statement_block == symbol;
    }
    return false; // LCOV_EXCL_LINE
}

static RcnCount evaluateNodeWeightJavaScriptImpl(
    TSNode node,
    NodeEvalTrace* trace
) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    switch (sym) {
        case sym_else_clause:
            trace->idxLastElse = trace->idx;
            weight += 1;
            break;
        case sym_if_statement:
            if (trace->idxLastElse != (trace->idx - 2)) {
                weight += 1;
            }
            break;
        case sym_do_statement:
            weight += 2;
            break;
        case sym_variable_declaration:
        case sym_lexical_declaration:
        case sym_using_declaration:
            if (!isJsForInitializerDeclaration(node)
                && !isJsExportedDeclaration(node)) {
                weight += 1;
            }
            break;
        case sym_function_declaration:
        case sym_generator_function_declaration:
        case sym_class_declaration:
            if (!isJsExportedDeclaration(node)) {
                weight += 1;
            }
            break;
        case sym_labeled_statement:
            if (isJsLabeledBlock(node)) {
                weight += 1;
            }
            break;
        case sym_expression_statement:
        case sym_import_statement:
        case sym_export_statement:
        case sym_debugger_statement:
        case sym_switch_statement:
        case sym_for_statement:
        case sym_for_in_statement:
        case sym_while_statement:
        case sym_try_statement:
        case sym_with_statement:
        case sym_break_statement:
        case sym_continue_statement:
        case sym_return_statement:
        case sym_throw_statement:
        case sym_switch_case:
        case sym_switch_default:
        case sym_catch_clause:
        case sym_finally_clause:
        case sym_method_definition:
        case sym_field_definition:
        case sym_class_static_block:
        case sym_decorator:
            weight += 1;
            break;
        default:
            break;
    }

    return weight;
}

void evaluateNodeJavaScript(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightJavaScriptImpl(node, trace);
    trace->idx++;
}
