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
#include <stdint.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "reckon_export.h"
#include "evaluation.h"

RECKON_NO_EXPORT const TSLanguage* tree_sitter_bash(void);

/**
 * These are the symbol identifiers as defined by the Bash language parser
 * of tree-sitter. We have only copied the symbol identifiers that we are
 * interested in evaluating or counting. Others do not contribute to the weight
 * of a node in the AST.
 */
enum SymbolIdentifiersBash {
    sym_for_statement = 179,
    sym_c_style_for_statement = 180,
    sym_while_statement = 189,
    sym_if_statement = 191,
    sym_elif_clause = 192,
    sym_else_clause = 193,
    sym_case_statement = 194,
    sym_case_item = 195,
    sym_last_case_item = 196,
    sym_function_definition = 197,
    sym_compound_statement = 198,
    sym_subshell = 199,
    sym_pipeline = 200,
    sym_declaration_command = 205,
    sym_unset_command = 206,
    sym_command = 207,
    sym_variable_assignment = 209,
};

TSParser* createParserBash(void) {
    TSParser* parser = ts_parser_new();
    if (parser) {
        if (!ts_parser_set_language(parser, tree_sitter_bash())) {
            // LCOV_EXCL_START
            ts_parser_delete(parser);
            return NULL;
            // LCOV_EXCL_STOP
        }
    }
    return parser;
}

static RcnCount evaluateNodeWeightBashImpl(
    TSNode node,
    NodeEvalTrace* trace
) {
    RcnCount weight = 0;
    TSSymbol sym = ts_node_grammar_symbol(node);
    const uint64_t line = currentLine(node);
    bool hitAny = true;
    switch (sym) {
        case sym_variable_assignment: {
            TSNode parent = ts_node_parent(node);
            if ((!ts_node_is_null(parent)
                && ts_node_grammar_symbol(parent) == sym_declaration_command)
                || trace->lnLastAny == currentLine(node)) {
                break;
            }
            weight += 1;
            break;
        }
        case sym_command: {
            if (trace->lnLastAny != currentLine(node)
                || trace->lnLastCaseItem == currentLine(node)) {
                weight += 1;
            }
            break;
        }
        case sym_compound_statement: {
            if (trace->lnLastFuncDef != currentLine(node)
                && trace->idxLastWhileSym != currentLine(node)) {
                weight += 1;
            }
            break;
        }
        case sym_function_definition: {
            trace->lnLastFuncDef = currentLine(node);
            weight += 1;
            break;
        }
        case sym_pipeline: {
            weight += 1;
            break;
        }
        case sym_case_item: {
            trace->lnLastCaseItem = currentLine(node);
            weight += 1;
            break;
        }
        case sym_last_case_item: {
            trace->lnLastCaseItem = currentLine(node);
            weight += 1;
            break;
        }
        case sym_while_statement: {
            trace->idxLastWhileSym = currentLine(node);
            weight += 1;
            break;
        }
        case sym_declaration_command:
        case sym_unset_command:
        case sym_if_statement:
        case sym_elif_clause:
        case sym_else_clause:
        case sym_for_statement:
        case sym_c_style_for_statement:
        case sym_case_statement:
        case sym_subshell:
            weight += 1;
            break;
        default:
            hitAny = false;
            break;
    }
    if (hitAny) {
        trace->lnLastAny = line;
    }
    return weight;
}

void evaluateNodeBash(TSNode node, NodeEvalTrace* trace) {
    trace->result->count += evaluateNodeWeightBashImpl(node, trace);
    trace->idx++;
}
