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

#include <stdint.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "reckon_export.h"
#include "evaluation.h"

RECKON_NO_EXPORT const TSLanguage* tree_sitter_c(void);

/**
 * These are the symbol identifiers as defined by the C language parser
 * of tree-sitter. We have only copied the symbol identifiers that we are
 * interested in evaluating or counting. Others do not contribute to the weight
 * of a node in the AST.
 */
enum SymbolIdentifiersC {
    sym_preproc_directive = 19,
    sym_preproc_include = 164,
    sym_preproc_def = 165,
    sym_preproc_function_def = 166,
    sym_preproc_if = 169,
    sym_preproc_ifdef = 170,
    sym_preproc_else = 171,
    sym_preproc_elif = 172,
    sym_preproc_elifdef = 173,
    sym_function_definition = 196,
    sym__old_style_function_definition = 197,
    sym_declaration = 198,
    sym_type_definition = 199,
    sym__type_definition_type = 200,
    sym__type_definition_declarators = 201,
    sym__declaration_modifiers = 202,
    sym__declaration_specifiers = 203,
    sym_linkage_specification = 204,
    sym_attribute_specifier = 205,
    sym_attribute = 206,
    sym_declaration_list = 213,
    sym__declarator = 214,
    sym__declaration_declarator = 215,
    sym__type_declarator = 217,
    sym__abstract_declarator = 218,
    sym_attributed_declarator = 223,
    sym_attributed_type_declarator = 225,
    sym_type_specifier = 245,
    sym_enum_specifier = 247,
    sym_struct_specifier = 249,
    sym_union_specifier = 250,
    sym_field_declaration = 253,
    sym_enumerator = 256,
    sym_attributed_statement = 261,
    sym_statement = 262,
    sym__top_level_statement = 263,
    sym_labeled_statement = 264,
    sym__top_level_expression_statement = 265,
    sym_expression_statement = 266,
    sym_if_statement = 267,
    sym_else_clause = 268,
    sym_switch_statement = 269,
    sym_case_statement = 270,
    sym_while_statement = 271,
    sym_do_statement = 272,
    sym_for_statement = 273,
    sym_return_statement = 275,
    sym_break_statement = 276,
    sym_continue_statement = 277,
    sym_goto_statement = 278,
    sym_expression = 283
};

TSParser* createParserC(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_c())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

static RcnCount evaluateNodeWeightCimpl(TSNode node, NodeEvalTrace* trace) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    switch (sym) {
        case sym_for_statement:
            trace->idxLastForSym = trace->idx;
            weight += 1;
            break;
        case sym_declaration:
            trace->lnLastDecl = currentLine(node);
            // Check if the following is present:
            //   for_statement
            //   for
            //   (
            //   declaration
            if (trace->idxLastForSym == (trace->idx - 3)) {
                // Do not count variable declarations inside for-statement
                break;
            }
            weight += 1;
            break;
        case sym_do_statement:
            weight += 2;
            break;
        case sym_type_definition:
            trace->idxLastTypeDef = trace->idx;
            weight += 1;
            break;
        case sym_struct_specifier:
            if (trace->idxLastTypeDef == (trace->idx - 2)) {
                break;
            }
            if (trace->lnLastDecl == currentLine(node)) {
                break;
            }
            if (trace->lnLastExpr == currentLine(node)) {
                break;
            }
            weight += 1;
            break;
        case sym_enum_specifier:
        case sym_union_specifier:
            if (trace->lnLastDecl == currentLine(node)) {
                break;
            }
            weight += 1;
            break;
        case sym__top_level_expression_statement:
        case sym_expression_statement:
            trace->lnLastExpr = currentLine(node);
            weight += 1;
            break;
        case sym_if_statement:
            // else-if counts as one
            // Nodes are: else_clause -> else -> if_statement
            if (trace->idxLastElse == (trace->idx - 2)) {
                break;
            }
            weight += 1;
            break;
        case sym_else_clause:
            trace->idxLastElse = trace->idx;
            weight += 1;
            break;
        case sym_preproc_directive:
        case sym_preproc_include:
        case sym_preproc_def:
        case sym_preproc_function_def:
        case sym_preproc_if:
        case sym_preproc_ifdef:
        case sym_preproc_else:
        case sym_preproc_elif:
        case sym_preproc_elifdef:
        case sym_function_definition:
        case sym__old_style_function_definition:
        case sym__type_definition_type:
        case sym__type_definition_declarators:
        case sym__declaration_modifiers:
        case sym__declaration_specifiers:
        case sym_linkage_specification:
        case sym_attribute_specifier:
        case sym_attribute:
        case sym_declaration_list:
        case sym__declarator:
        case sym__declaration_declarator:
        case sym__type_declarator:
        case sym__abstract_declarator:
        case sym_attributed_declarator:
        case sym_attributed_type_declarator:
        case sym_type_specifier:
        case sym_field_declaration:
        case sym_enumerator:
        case sym_attributed_statement:
        case sym_statement:
        case sym__top_level_statement:
        case sym_labeled_statement:
        case sym_switch_statement:
        case sym_case_statement:
        case sym_while_statement:
        case sym_return_statement:
        case sym_break_statement:
        case sym_continue_statement:
        case sym_goto_statement:
        case sym_expression:
            weight += 1;
            break;
        default:
            break;
    }
    return weight;
}

void evaluateNodeC(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightCimpl(node, trace);
    trace->idx++;
}
