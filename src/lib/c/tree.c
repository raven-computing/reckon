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
#include "evaluation.h"

#ifdef RECKON_DEBUG
#define RECKON_LOG_SYNTAX_ERRORS \
    RCN_LOG_DBG("[ERROR] Syntax error in file detected") \
    traverseTree(rootNode, showNodeSyntaxError, trace);

/**
 * A `NodeVisitor` function that logs syntax errors for a node.
 * Must only have an effect when the debug mode is enabled.
 */
static void showNodeSyntaxError(TSNode node, NodeEvalTrace* trace) {
    if (ts_node_is_error(node)) {
        RCN_LOG_DBG("[ERROR] The above node has a syntax error")
    } else if (ts_node_is_missing(node)) {
        RCN_LOG_DBG(
            "[ERROR] The above node is missing and produces a syntax error"
        )
    }
}
#else
#define RECKON_LOG_SYNTAX_ERRORS
#endif

enum TraversalState {
    DESCEND,
    NEXT_SIBLING,
    ASCEND
};

static inline TSInputEncoding mapInputEncoding(TextEncoding encoding) {
    switch (encoding) {
        case TextEncodingUTF8:
            return TSInputEncodingUTF8;
        case TextEncodingUTF16LE:
            return TSInputEncodingUTF16LE;
        case TextEncodingUTF16BE:
            return TSInputEncodingUTF16BE;
        // LCOV_EXCL_START
        default:
            return TSInputEncodingUTF8;
        // LCOV_EXCL_STOP
    }
}

void traverseTree(TSNode root, NodeVisitor visitor, NodeEvalTrace* trace) {
    TSTreeCursor cursor = ts_tree_cursor_new(root);
    enum TraversalState state = DESCEND;
    for (;;) {
        TSNode node = ts_tree_cursor_current_node(&cursor);
        if (state == DESCEND) {
            if (visitor) {
                RCN_LOG_DBG_NODE(node);
                visitor(node, trace);
            }
            if (ts_tree_cursor_goto_first_child(&cursor)) {
                state = DESCEND;
                continue;
            }
            state = NEXT_SIBLING;
        }
        if (state == NEXT_SIBLING) {
            if (ts_tree_cursor_goto_next_sibling(&cursor)) {
                state = DESCEND;
                continue;
            }
            state = ASCEND;
        }
        if (state == ASCEND) {
            if (!ts_tree_cursor_goto_parent(&cursor)) {
                break;
            }
            state = NEXT_SIBLING;
        }
    }
    ts_tree_cursor_delete(&cursor);
}

RcnResultState evaluateSourceTree(
    RcnSourceText source,
    RcnTextFormat language,
    NodeVisitor evaluator,
    NodeEvalTrace* trace
) {
    RcnResultState state = {0};
    if (source.size > UINT32_MAX) {
        state.errorCode = RCN_ERR_INPUT_TOO_LARGE;
        state.errorMessage = "Source input exceeds maximum supported size";
        return state;
    }
    TSParser* parser = createParser(language);
    if (!parser) {
        state.errorCode = RCN_ERR_UNSUPPORTED_FORMAT;
        state.errorMessage = "The input language is not supported";
        return state;
    }
    TextEncoding encoding = detectEncoding(source);

    TSTree* tree = ts_parser_parse_string_encoding(
        parser,
        NULL,
        source.text,
        (uint32_t) source.size,
        mapInputEncoding(encoding)
    );

    TSNode rootNode = ts_tree_root_node(tree);

    if (ts_node_has_error(rootNode)) {
        RECKON_LOG_SYNTAX_ERRORS
        ts_tree_delete(tree);
        ts_parser_delete(parser);
        state.errorCode = RCN_ERR_SYNTAX_ERROR;
        state.errorMessage = "Syntax error detected in source code";
        return state;
    }

    traverseTree(rootNode, evaluator, trace);

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    state.ok = true;
    return state;
}

uint64_t currentLine(TSNode node) {
    return (uint64_t) ts_node_start_point(node).row + 1;
}
