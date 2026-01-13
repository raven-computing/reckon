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

RECKON_NO_EXPORT const TSLanguage* tree_sitter_java(void);

/**
 * These are the symbol identifiers as defined by the Java language parser
 * of tree-sitter. We have only copied the symbol identifiers that we are
 * interested in evaluating or counting. Others do not contribute to the weight
 * of a node in the AST.
 */
enum SymbolIdentifiersJava {
  anon_sym_DASH_GT = 56,
  anon_sym_when = 76,
  anon_sym_else = 91,
  anon_sym_open = 94,
  anon_sym_module = 95,
  anon_sym_requires = 96,
  anon_sym_transitive = 97,
  anon_sym_exports = 99,
  anon_sym_to = 100,
  anon_sym_opens = 101,
  anon_sym_uses = 102,
  anon_sym_provides = 103,
  anon_sym_with = 104,
  sym_expression = 147,
  sym_switch_expression = 174,
  sym_switch_label = 178,
  sym_pattern = 179,
  sym_type_pattern = 180,
  sym_record_pattern = 181,
  sym_record_pattern_body = 182,
  sym_record_pattern_component = 183,
  sym_guard = 184,
  sym_statement = 185,
  sym_expression_statement = 187,
  sym_assert_statement = 189,
  sym_do_statement = 190,
  sym_break_statement = 191,
  sym_continue_statement = 192,
  sym_return_statement = 193,
  sym_yield_statement = 194,
  sym_synchronized_statement = 195,
  sym_throw_statement = 196,
  sym_try_statement = 197,
  sym_catch_clause = 198,
  sym_finally_clause = 201,
  sym_try_with_resources_statement = 202,
  sym_if_statement = 205,
  sym_while_statement = 206,
  sym_for_statement = 207,
  sym_enhanced_for_statement = 208,
  sym_marker_annotation = 210,
  sym_annotation = 211,
  sym_declaration = 216,
  sym_module_declaration = 217,
  sym_module_directive = 219,
  sym_requires_module_directive = 220,
  sym_requires_modifier = 221,
  sym_exports_module_directive = 222,
  sym_opens_module_directive = 223,
  sym_uses_module_directive = 224,
  sym_provides_module_directive = 225,
  sym_package_declaration = 226,
  sym_import_declaration = 227,
  sym_enum_declaration = 229,
  sym_enum_constant = 232,
  sym_class_declaration = 233,
  sym_permits = 241,
  sym_static_initializer = 243,
  sym_constructor_declaration = 244,
  sym__constructor_declarator = 245,
  sym_explicit_constructor_invocation = 247,
  sym_field_declaration = 249,
  sym_record_declaration = 250,
  sym_annotation_type_declaration = 251,
  sym_annotation_type_element_declaration = 253,
  sym_interface_declaration = 255,
  sym_constant_declaration = 258,
  sym__method_declarator = 272,
  sym_local_variable_declaration = 278,
  sym_method_declaration = 279,
  sym_compact_constructor_declaration = 280,
};

TSParser* createParserJava(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_java())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

static RcnCount evaluateNodeWeightJavaImpl(TSNode node, NodeEvalTrace* trace) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    switch (sym) {
        case anon_sym_DASH_GT:
            trace->lnLastArrow = currentLine(node);
            break;
        case anon_sym_else:
            trace->idxLastElse = trace->idx;
            weight += 1;
            break;
        case sym_switch_label:
            trace->lnLastSwitchLabel = currentLine(node);
            weight += 1;
            break;
        case sym_expression_statement: {
            const uint64_t line = currentLine(node);
            if ((trace->lnLastSwitchLabel == line)
             && (trace->lnLastArrow == line)) {
                break;
            }
            weight += 1;
            break;
        }
        case sym_if_statement:
            // else-if counts as one
            if (trace->idxLastElse == (trace->idx - 1)) {
                break;
            }
            weight += 1;
            break;
        case sym_local_variable_declaration:
            // Check if the following is present:
            //   for_statement
            //   for
            //   (
            //   local_variable_declaration
            if (trace->idxLastForSym == (trace->idx - 3)) {
                // Do not count variable declarations inside for-statement
                break;
            }
            weight += 1;
            break;
        case sym_do_statement:
            weight += 2;
            break;
        case sym_for_statement:
            trace->idxLastForSym = trace->idx;
            FALLTHROUGH;
        case anon_sym_when:
        case anon_sym_open:
        case anon_sym_module:
        case anon_sym_requires:
        case anon_sym_transitive:
        case anon_sym_exports:
        case anon_sym_to:
        case anon_sym_opens:
        case anon_sym_uses:
        case anon_sym_provides:
        case anon_sym_with:
        case sym_expression:
        case sym_switch_expression:
        case sym_pattern:
        case sym_type_pattern:
        case sym_record_pattern:
        case sym_record_pattern_body:
        case sym_record_pattern_component:
        case sym_guard:
        case sym_statement:
        case sym_assert_statement:
        case sym_break_statement:
        case sym_continue_statement:
        case sym_return_statement:
        case sym_yield_statement:
        case sym_synchronized_statement:
        case sym_throw_statement:
        case sym_try_statement:
        case sym_catch_clause:
        case sym_finally_clause:
        case sym_try_with_resources_statement:
        case sym_while_statement:
        case sym_enhanced_for_statement:
        case sym_marker_annotation:
        case sym_annotation:
        case sym_declaration:
        case sym_module_declaration:
        case sym_module_directive:
        case sym_requires_module_directive:
        case sym_requires_modifier:
        case sym_exports_module_directive:
        case sym_opens_module_directive:
        case sym_uses_module_directive:
        case sym_provides_module_directive:
        case sym_package_declaration:
        case sym_import_declaration:
        case sym_enum_declaration:
        case sym_enum_constant:
        case sym_class_declaration:
        case sym_permits:
        case sym_static_initializer:
        case sym_constructor_declaration:
        case sym__constructor_declarator:
        case sym_explicit_constructor_invocation:
        case sym_field_declaration:
        case sym_record_declaration:
        case sym_annotation_type_declaration:
        case sym_annotation_type_element_declaration:
        case sym_interface_declaration:
        case sym_constant_declaration:
        case sym__method_declarator:
        case sym_method_declaration:
        case sym_compact_constructor_declaration:
            weight += 1;
            break;
        default:
            break;
    }
    return weight;
}

void evaluateNodeJava(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightJavaImpl(node, trace);
    trace->idx++;
}
