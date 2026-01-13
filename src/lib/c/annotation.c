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

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "evaluation.h"

/**
 * The initial capacity for `LineCommentBuffer`.
 */
static const size_t BUF_CAP_INIT = 32;

/**
 * The initial capacity for the `LineCommentBuffer.symbolTypes` buffer.
 */
static const size_t BUF_SYM_CAP_INIT = 4;

/**
 * The factor by which `LineCommentBuffer` grows when more space is needed.
 */
static const size_t BUF_CAP_GROW_FACTOR = 2;

/**
 * Buffer to accumulate per-line annotation comments.
 * A source code comment might look like: ` // +2 (type 1, type 2)`
 */
typedef struct {
    char* ptr;
    size_t size;
    size_t capacity;
    RcnCount weight;
    char** symbolTypes;
    size_t symbolCount;
    size_t symbolCapacity;
} LineCommentBuffer;

/**
 * Concrete type to be used in place of the opaque `NodeEvalContext`.
 * Keeps track of the per-line annotation comments and the AST-node evaluator.
 */
typedef struct {
    NodeVisitor evaluator;
    LineCommentBuffer* lines;
    size_t lineCount;
    RcnTextFormat language;
} AnnotationContext;

/**
 * Ensures that the given buffer has enough capacity to fit an additional
 * element of size `additional`.
 */
static bool linebufferReserve(LineCommentBuffer* buffer, size_t additional) {
    assert(buffer != NULL);
    const size_t requiredCapacity = buffer->size + additional;
    if (buffer->capacity >= requiredCapacity) {
        return true;
    }
    size_t newCapacity = buffer->capacity;
    if (newCapacity == 0) {
        newCapacity = BUF_CAP_INIT;
    }
    while (newCapacity < requiredCapacity) {
        newCapacity *= BUF_CAP_GROW_FACTOR;
    }
    char* reallocatedData = realloc(buffer->ptr, newCapacity);
    if (!reallocatedData) {
        return false;
    }
    buffer->ptr = reallocatedData;
    buffer->capacity = newCapacity;
    return true;
}

/**
 * Appends the given string to the text data of the line buffer.
 */
static void linebufferAppend(LineCommentBuffer* buffer, const char* string) {
    if (!string) {
        return;
    }
    const size_t length = strlen(string);
    if (!linebufferReserve(buffer, length + 1)) {
        return;
    }
    memcpy(buffer->ptr + buffer->size, string, length);
    buffer->size += length;
    buffer->ptr[buffer->size] = '\0';
}

/**
 * Saves a counted symbol type in the line buffer.
 */
static void linebufferRecordType(
    LineCommentBuffer* buffer,
    const char* symbolName
) {
    if (!symbolName) {
        return;
    }
    assert(buffer != NULL);
    if (buffer->symbolCount >= buffer->symbolCapacity) {
        size_t newCapacity = (
            buffer->symbolCapacity
            ? buffer->symbolCapacity * BUF_CAP_GROW_FACTOR
            : BUF_SYM_CAP_INIT
        );
        char** reallocatedData = (char**) realloc(
            (void*) buffer->symbolTypes,
            newCapacity * sizeof(char*)
        );
        if (!reallocatedData) {
            return;
        }
        buffer->symbolTypes = reallocatedData;
        buffer->symbolCapacity = newCapacity;
    }
    const size_t symbolLength = strlen(symbolName);
    char* symbolNameCopy = malloc(symbolLength + 1);
    if (!symbolNameCopy) {
        return;
    }
    memcpy(symbolNameCopy, symbolName, symbolLength + 1);
    for (char* character = symbolNameCopy; *character; ++character) {
        if (*character == '_') {
            *character = ' ';
        }
    }
    buffer->symbolTypes[buffer->symbolCount++] = symbolNameCopy;
}

/**
 * Frees all symbol type strings recorded in the given line buffer.
 */
static void freeSymbolTypes(LineCommentBuffer* buffer) {
    assert(buffer != NULL && buffer->symbolTypes != NULL);
    for (size_t i = 0; i < buffer->symbolCount; ++i) {
        free(buffer->symbolTypes[i]);
    }
    free((void*) buffer->symbolTypes);
    buffer->symbolTypes = NULL;
    buffer->symbolCount = 0;
    buffer->symbolCapacity = 0;
}

/**
 * Finalizes source evaluation and constructs line comment strings from
 * the numeric weight values and their correpsponding symbol names in
 * the given buffers. This transforms the computed intermediate buffers
 * to the final textual form. Intermediate buffers are freed afterwards.
 */
static void finalizeLineComments(AnnotationContext* ctx) {
    LineCommentBuffer* lines = ctx->lines;
    const char* symbolNameSeparator = ", ";
    const size_t symbolNameSeparatorLength = strlen(symbolNameSeparator);
    const char* space = " ";
    const char* commentText = getInlineSourceCommentString(ctx->language);
    const char* prefix = " +";
    const char* open = " (";
    const char* close = ")";
    for (size_t i = 0; i < ctx->lineCount; ++i) {
        LineCommentBuffer* buffer = &lines[i];
        if (buffer->symbolCount == 0) {
            continue;
        }
        char weightBuffer[32] = {0};
        (void) snprintf(
            weightBuffer,
            sizeof(weightBuffer),
            "%llu",
            (unsigned long long) buffer->weight
        );
        size_t namesLength = 0;
        for (size_t j = 0; j < buffer->symbolCount; ++j) {
            namesLength += strlen(buffer->symbolTypes[j]);
            if (j + 1 < buffer->symbolCount) {
                namesLength += symbolNameSeparatorLength;
            }
        }

        size_t commentStringLength = (
            strlen(space)
            + strlen(commentText)
            + strlen(prefix)
            + strlen(weightBuffer)
            + strlen(open)
            + strlen(close)
            + namesLength
            + 1
        );
        if (!linebufferReserve(buffer, commentStringLength)) {
            freeSymbolTypes(buffer);
            continue;
        }
        linebufferAppend(buffer, space);
        linebufferAppend(buffer, commentText);
        linebufferAppend(buffer, prefix);
        linebufferAppend(buffer, weightBuffer);
        linebufferAppend(buffer, open);
        for (size_t j = 0; j < buffer->symbolCount; ++j) {
            linebufferAppend(buffer, buffer->symbolTypes[j]);
            if ((j + 1) < buffer->symbolCount) {
                linebufferAppend(buffer, symbolNameSeparator);
            }
        }
        linebufferAppend(buffer, close);
        freeSymbolTypes(buffer);
    }
}

/**
 * Indicates whether the character at the given index in `sourceCode` is
 * a newline character. Returns 0 if not a newline, 1 if newline consists
 * of LF only, and 2 if newline consists of CRLF.
 */
static inline int checkIsNewline(
    const char* sourceCode,
    size_t length,
    size_t index
) {
    const char character0 = sourceCode[index];
    if (character0 == '\n') {
        return 1;
    }
    if (character0 == '\r') {
        if (((index + 1) < length) && sourceCode[index + 1] == '\n') {
            return 2;
        }
    }
    return 0;
}

/**
 * Builds the annotation output by merging per-line string comments
 * in-between source code lines and their line breaks.
 */
static size_t mergeSourceAnnotations(
    const char* sourceCode,
    size_t sourceLength,
    char* annotated,
    AnnotationContext* ctx
) {
    size_t charIndex = 0;
    size_t lineIndex = 0;
    const size_t lineCount = ctx->lineCount;
    for (size_t i = 0; i < sourceLength; ++i) {
        char character = sourceCode[i];
        int nlType = checkIsNewline(sourceCode, sourceLength, i);
        if (nlType) {
            if (lineIndex < lineCount && ctx->lines[lineIndex].size) {
                memcpy(
                    annotated + charIndex,
                    ctx->lines[lineIndex].ptr,
                    ctx->lines[lineIndex].size
                );
                charIndex += ctx->lines[lineIndex].size;
            }
            if (nlType == 2) {
                annotated[charIndex++] = sourceCode[i++]; // CR
            }
            annotated[charIndex++] = sourceCode[i]; // LF
            lineIndex++;
            continue;
        }
        annotated[charIndex++] = character;
    }
    // If last line had no newline, append its comment at the end
    if ((lineIndex < lineCount) && ctx->lines[lineIndex].size) {
        memcpy(
            annotated + charIndex,
            ctx->lines[lineIndex].ptr,
            ctx->lines[lineIndex].size
        );
        charIndex += ctx->lines[lineIndex].size;
    }
    annotated[charIndex++] = '\0';
    return charIndex;
}

NodeEvalContext* createNodeEvalContextAnnotation(
    RcnTextFormat language,
    size_t lineCount
) {
    AnnotationContext* ctx = malloc(sizeof(AnnotationContext));
    if (!ctx) {
        return NULL;
    }
    ctx->evaluator = createEvaluationFunction(language);
    if (!ctx->evaluator) {
        free(ctx);
        return NULL;
    }
    ctx->lineCount = lineCount;
    ctx->language = language;
    ctx->lines = calloc(lineCount, sizeof(LineCommentBuffer));
    if (!ctx->lines) {
        free(ctx);
        return NULL;
    }
    return (NodeEvalContext*) ctx;
}

void freeNodeEvalContextAnnotation(NodeEvalContext* ctx) {
    if (!ctx) {
        return;
    }
    AnnotationContext* annotationCtx = (AnnotationContext*) ctx;
    for (size_t i = 0; i < annotationCtx->lineCount; ++i) {
        free(annotationCtx->lines[i].ptr);
    }
    free(annotationCtx->lines);
    free(annotationCtx);
}

void annotateLineWithNodeType(TSNode node, NodeEvalTrace* trace) {
    AnnotationContext* ctx = (AnnotationContext*) trace->ctx;
    if (!ctx || !ctx->lines) {
        return;
    }
    TSPoint start = ts_node_start_point(node);
    const uint32_t row = start.row;
    if (row >= ctx->lineCount) {
        return;
    }
    assert(ctx->evaluator != NULL);
    const char* symbolName = ts_node_type(node);
    RcnCountResult result = {0};
    trace->result = &result;
    ctx->evaluator(node, trace);
    // Weight assigned during evaluation
    const RcnCount symbolWeight = trace->result->count;
    if (symbolWeight) {
        LineCommentBuffer* buffer = &ctx->lines[row];
        buffer->weight += symbolWeight;
        linebufferRecordType(buffer, symbolName);
    }
    trace->result = NULL; // Reset
}

RcnSourceText buildAnnotatedSource(
    const char* sourceCode,
    const NodeEvalTrace* trace
) {
    AnnotationContext* ctx = (AnnotationContext*) trace->ctx;
    finalizeLineComments(ctx);

    size_t spaceForComments = 0;
    for (size_t i = 0; i < ctx->lineCount; ++i) {
        spaceForComments += ctx->lines[i].size;
    }
    const size_t sourceLength = strlen(sourceCode);
    const size_t annotatedSourceLength = sourceLength + spaceForComments + 1;
    char* annotated = malloc(annotatedSourceLength);
    if (!annotated) {
        return (RcnSourceText){0};
    }
    const size_t nCharsWritten = mergeSourceAnnotations(
        sourceCode,
        sourceLength,
        annotated,
        ctx
    );
    assert(nCharsWritten == annotatedSourceLength);
    return (RcnSourceText){
        .text = annotated,
        .size = nCharsWritten - 1
    };
}
