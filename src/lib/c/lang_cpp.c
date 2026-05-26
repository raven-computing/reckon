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

RECKON_NO_EXPORT const TSLanguage* tree_sitter_cpp(void);

/**
 * These are the symbol identifiers as defined by the C++ language parser
 * of tree-sitter. We have only copied the symbol identifiers that we are
 * interested in evaluating or counting. Others do not contribute to the weight
 * of a node in the AST.
 */
enum SymbolIdentifiersCpp {
    sym_preproc_directive = 19,
    sym_preproc_include = 222,
    sym_preproc_def = 223,
    sym_preproc_function_def = 224,
    sym_preproc_if = 227,
    sym_preproc_ifdef = 228,
    sym_preproc_else = 229,
    sym_preproc_elif = 230,
    sym_preproc_elifdef = 231,
    sym_function_definition = 254,
    sym_declaration = 255,
    sym_type_definition = 256,
    sym__type_definition_type = 257,
    sym__type_definition_declarators = 258,
    sym__declaration_modifiers = 259,
    sym__declaration_specifiers = 260,
    sym_linkage_specification = 261,
    sym_attribute_specifier = 262,
    sym_attribute = 263,
    sym_declaration_list = 270,
    sym__declarator = 271,
    sym__type_declarator = 273,
    sym__abstract_declarator = 274,
    sym_attributed_declarator = 279,
    sym_attributed_type_declarator = 281,
    sym_type_specifier = 299,
    sym_enum_specifier = 301,
    sym_struct_specifier = 303,
    sym_union_specifier = 304,
    sym_field_declaration = 307,
    sym_enumerator = 309,
    sym_attributed_statement = 312,
    sym_statement = 313,
    sym__top_level_statement = 314,
    sym_labeled_statement = 315,
    sym__top_level_expression_statement = 316,
    sym_expression_statement = 317,
    sym_if_statement = 318,
    sym_else_clause = 319,
    sym_switch_statement = 320,
    sym_case_statement = 321,
    sym_while_statement = 322,
    sym_do_statement = 323,
    sym_for_statement = 324,
    sym_return_statement = 326,
    sym_break_statement = 327,
    sym_continue_statement = 328,
    sym_goto_statement = 329,
    sym_expression = 334,
    sym_class_specifier = 379,
    sym_inline_method_definition = 400,
    sym_constructor_or_destructor_definition = 405,
    sym_friend_declaration = 410,
    sym_template_declaration = 386,
    sym_template_instantiation = 387,
    sym_namespace_definition = 430,
    sym_namespace_alias_definition = 431,
    sym_using_declaration = 434,
    sym_alias_declaration = 435,
    sym_static_assert_declaration = 436,
    sym_concept_definition = 437,
    sym_for_range_loop = 438,
    sym_co_return_statement = 443,
    sym_co_yield_statement = 444,
    sym_throw_statement = 445,
    sym_try_statement = 446,
    sym_catch_clause = 447,
};

TSParser* createParserCpp(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_cpp())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

static RcnCount evaluateNodeWeightCppImpl(TSNode node, NodeEvalTrace* trace) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    switch (sym) {
        case sym_for_statement:
        case sym_for_range_loop:
            trace->idxLastForSym = trace->idx;
            weight += 1;
            break;
        case sym_declaration:
            trace->lnLastDecl = currentLine(node);
            // Do not count variable declarations inside for-statement
            // Check if the following is present:
            //   for_statement / for_range_loop
            //   for
            //   (
            //   declaration
            if (trace->idxLastForSym != (trace->idx - 3)) {
                weight += 1;
            }
            break;
        case sym_do_statement:
            weight += 2;
            break;
        case sym_type_definition:
            trace->idxLastTypeDef = trace->idx;
            weight += 1;
            break;
        case sym_struct_specifier:
        case sym_class_specifier:
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
            if (trace->idxLastTypeDef == (trace->idx - 2)
                || trace->lnLastDecl == currentLine(node)) {
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
        case sym__type_definition_type:
        case sym__type_definition_declarators:
        case sym__declaration_modifiers:
        case sym__declaration_specifiers:
        case sym_linkage_specification:
        case sym_attribute_specifier:
        case sym_attribute:
        case sym_declaration_list:
        case sym__declarator:
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
        case sym_inline_method_definition:
        case sym_constructor_or_destructor_definition:
        case sym_friend_declaration:
        case sym_template_declaration:
        case sym_template_instantiation:
        case sym_namespace_definition:
        case sym_namespace_alias_definition:
        case sym_using_declaration:
        case sym_alias_declaration:
        case sym_static_assert_declaration:
        case sym_concept_definition:
        case sym_co_return_statement:
        case sym_co_yield_statement:
        case sym_throw_statement:
        case sym_try_statement:
        case sym_catch_clause:
            weight += 1;
        default:
            break;
    }
    return weight;
}

void evaluateNodeCpp(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightCppImpl(node, trace);
    trace->idx++;
}
