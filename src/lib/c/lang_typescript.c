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

RECKON_NO_EXPORT const TSLanguage* tree_sitter_typescript(void);

/**
 * These are the symbol identifiers as defined by the TypeScript language
 * parser of tree-sitter. We have only copied the symbol identifiers that we
 * are interested in evaluating or counting. Others do not contribute to the
 * weight of a node in the AST.
 */
enum SymbolIdentifiersTypeScript {
    sym_hash_bang_line = 2,
    sym_export_statement = 167,
    sym_export_clause = 169,
    sym_declaration = 172,
    sym_import_statement = 174,
    sym_import_clause = 175,
    sym_namespace_import = 177,
    sym_statement = 181,
    sym_expression_statement = 182,
    sym_variable_declaration = 183,
    sym_lexical_declaration = 184,
    sym_variable_declarator = 185,
    sym_statement_block = 186,
    sym_else_clause = 187,
    sym_if_statement = 188,
    sym_switch_statement = 189,
    sym_for_statement = 190,
    sym_for_in_statement = 191,
    sym__for_header = 192,
    sym_while_statement = 193,
    sym_do_statement = 194,
    sym_try_statement = 195,
    sym_with_statement = 196,
    sym_break_statement = 197,
    sym_continue_statement = 198,
    sym_debugger_statement = 199,
    sym_return_statement = 200,
    sym_throw_statement = 201,
    sym_labeled_statement = 203,
    sym_switch_case = 205,
    sym_switch_default = 206,
    sym_catch_clause = 207,
    sym_finally_clause = 208,
    sym_parenthesized_expression = 209,
    sym_expression = 210,
    sym_primary_expression = 211,
    sym_yield_expression = 212,
    sym_class_declaration = 221,
    sym_function_expression = 223,
    sym_function_declaration = 224,
    sym_generator_function_declaration = 226,
    sym_arrow_function = 227,
    sym_call_expression = 231,
    sym_new_expression = 232,
    sym_await_expression = 233,
    sym_member_expression = 234,
    sym_assignment_expression = 236,
    sym_augmented_assignment_expression = 238,
    sym_ternary_expression = 242,
    sym_binary_expression = 243,
    sym_unary_expression = 244,
    sym_update_expression = 245,
    sym_sequence_expression = 246,
    sym_decorator = 253,
    sym_decorator_member_expression = 254,
    sym_decorator_call_expression = 255,
    sym_class_static_block = 258,
    sym_method_definition = 261,
    sym_public_field_definition = 266,
    sym_abstract_method_signature = 270,
    sym_abstract_class_declaration = 282,
};

TSParser* createParserTypeScript(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_typescript())) {
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

static bool isTsExportedDeclaration(TSNode node) {
    return parentHasSymbol(node, sym_export_statement);
}

static bool isTsForInitializerDeclaration(TSNode node) {
    return parentHasSymbol(node, sym_for_statement);
}

static bool isTsLabeledBlock(TSNode node) {
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

static RcnCount evaluateNodeWeightTypeScriptImpl(
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
            if (!isTsForInitializerDeclaration(node)
                && !isTsExportedDeclaration(node)) {
                weight += 1;
            }
            break;
        case sym_function_declaration:
        case sym_generator_function_declaration:
        case sym_class_declaration:
            if (!isTsExportedDeclaration(node)) {
                weight += 1;
            }
            break;
        case sym_labeled_statement:
            if (isTsLabeledBlock(node)) {
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
        case sym_public_field_definition:
        case sym_class_static_block:
        case sym_decorator:
        case sym_abstract_method_signature:
        case sym_abstract_class_declaration:
            weight += 1;
            break;
        default:
            break;
    }

    return weight;
}

void evaluateNodeTypeScript(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightTypeScriptImpl(node, trace);
    trace->idx++;
}
