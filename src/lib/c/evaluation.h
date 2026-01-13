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

/**
 * Internal functions and types for evaluating source text.
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define FALLTHROUGH __attribute__((fallthrough))
#else
#define FALLTHROUGH
#endif

#ifdef RECKON_DEBUG

void logDebugNode(TSNode node);
void logDebugMessage(const char* message);

#define RCN_LOG_DBG_NODE(node) logDebugNode(node);
#define RCN_LOG_DBG(message) logDebugMessage(message);
#else
#define RCN_LOG_DBG_NODE(node)
#define RCN_LOG_DBG(message)
#endif

/**
 * Opaque context type used during node evaluation.
 * Can be used by concrete `NodeVisitor` implementations to store
 * language-specific processing state.
 */
typedef struct NodeEvalContext NodeEvalContext;

/**
 * Type to trace evaluation information during AST traversal.
 */
typedef struct NodeEvalTrace {
    RcnCountResult* result;
    NodeEvalContext* ctx;
    uint64_t idx;
    uint64_t idxLastForSym;
    uint64_t idxLastElse;
    uint64_t idxLastTypeDef;
    uint64_t lnLastDecl;
    uint64_t lnLastExpr;
    uint64_t lnLastSwitchLabel;
    uint64_t lnLastArrow;
} NodeEvalTrace;

/**
 * Function pointer type for node visitor functions used during AST
 * traversal and evaluation. Such a function is called for every node in the
 * tree and should perform evaluation logic specific to the node type.
 */
typedef void (*NodeVisitor)(TSNode node, NodeEvalTrace* trace);

/**
 * Enumeration of supported text encodings.
 */
typedef enum TextEncoding {
  TextEncodingUTF8,
  TextEncodingUTF16LE,
  TextEncodingUTF16BE
} TextEncoding;

/**
 * Evaluates the AST of the given source code.
 * The specified `NodeVisitor` is used to evaluate every node in the tree. The
 * specified `NodeEvalTrace` can be passed by the caller to track
 * the evaluation state across nodes. The returned `RcnResultState` indicates
 * whether the evaluation was successful or if an error occurred.
 */
RcnResultState evaluateSourceTree(
    RcnSourceText source,
    RcnTextFormat language,
    NodeVisitor evaluator,
    NodeEvalTrace* trace
);

/**
 * Traverses the entire AST, starting at the given root node, calling the
 * specified `NodeVisitor` for each node. The specified `NodeEvalTrace` is
 * passed to the visitor function unaltered and can be used during the
 * evaluation of the tree node.
 */
void traverseTree(TSNode root, NodeVisitor visitor, NodeEvalTrace* trace);

/**
 * Allocates and creates a parser for source code in the specified
 * programming language. May return `NULL` if the specified language is not
 * supported or on error. Ownership of the returned parser is transferred to
 * the caller.
 */
TSParser* createParser(RcnTextFormat language);

/**
 * Returns a node evaluation function for the specified programming language.
 * May return `NULL` if the specified language is not supported. The returned
 * function pointer can be used as a `NodeVisitor` during AST traversal
 * and evaluation.
 */
NodeVisitor createEvaluationFunction(RcnTextFormat language);

/**
 * Returns a string that contains the characters that are used when writing an
 * inline comment in source code that is written in the
 * specified programming language. The caller does not own the returned string
 * and must not attempt to free it.
 */
const char* getInlineSourceCommentString(RcnTextFormat language);

/**
 * Allocates a new node evaluation context for an annotation operation.
 * Ownership of the returned context is transferred to the caller. It must be
 * freed with `freeNodeEvalContextAnnotation()`.
 */
NodeEvalContext* createNodeEvalContextAnnotation(
    RcnTextFormat language,
    size_t lineCount
);

/**
 * Frees the given node evaluation context and all of its resources.
 */
void freeNodeEvalContextAnnotation(NodeEvalContext* ctx);

/**
 * A `NodeVisitor` implementation that annotates lines in the evaluation trace
 * with the type of the given node and its logical line count.
 */
void annotateLineWithNodeType(TSNode node, NodeEvalTrace* trace);

/**
 * Creates an annotated source text representation from the given raw
 * source code and the evaluation trace. Ownership of the
 * returned `RcnSourceText` is transferred to the caller and he must
 * free it with the `rcnFreeSourceText()` function.
 */
RcnSourceText buildAnnotatedSource(
    const char* sourceCode,
    const NodeEvalTrace* trace
);

/**
 * Checks whether the given source text starts with a UTF-8 BOM.
 */
bool hasUTF8BOM(RcnSourceText source);

/**
 * Detects the text encoding of the given input source.
 * 
 * Supports detection of UTF-8 (with or without BOM), UTF-16LE and UTF-16BE.
 * By default, UTF-8 is assumed if no BOM is present.
 */
TextEncoding detectEncoding(RcnSourceText source);

/**
 * Returns the physical line number that corresponds to the given node.
 * The line number is one-based.
 */
uint64_t currentLine(TSNode node);

#ifdef __cplusplus
}
#endif
