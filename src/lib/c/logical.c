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

#include "reckon/reckon.h"
#include "evaluation.h"
#include "fileio.h"

RcnCountResult rcnCountLogicalLines(
    RcnTextFormat language,
    RcnSourceText sourceCode
) {
    RcnCountResult result = {0};
    if (!sourceCode.text) {
        result.state.errorCode = RCN_ERR_INVALID_INPUT;
        result.state.errorMessage = "Source code input must not be NULL";
        return result;
    }
    NodeEvalTrace trace = {0};
    trace.result = &result;

    NodeVisitor evaluator = createEvaluationFunction(language);
    if (evaluator == NULL) {
        result.state.errorCode = RCN_ERR_UNSUPPORTED_FORMAT;
        result.state.errorMessage = (
            "The input format or programming language is not supported"
        );
        return result;
    }
    RcnResultState evalState = evaluateSourceTree(
        sourceCode,
        language,
        evaluator,
        &trace
    );
    result.state = evalState;
    return result;
}

RcnSourceText rcnMarkLogicalLinesInFile(const char* path) {
    RcnSourceFile* file = newSourceFile(path);
    if (!file) {
        return (RcnSourceText){0};
    }
    SourceFormatDetection detected = detectSourceFormat(file);
    if (!detected.isProgrammingLanguage || !readSourceFileContent(file)) {
        freeSourceFile(file);
        return (RcnSourceText){0};
    }
    RcnSourceText annotated = rcnMarkLogicalLinesInSourceText(
        detected.format,
        file->content
    );
    freeSourceFile(file);
    return annotated;
}

RcnSourceText rcnMarkLogicalLinesInSourceText(
    RcnTextFormat language,
    RcnSourceText sourceCode
) {
    RcnSourceText resultText = {0};
    if (!sourceCode.text) {
        return resultText;
    }
    TextEncoding encoding = detectEncoding(sourceCode);
    if (encoding != TextEncodingUTF8) {
        return resultText;
    }
    if (hasUTF8BOM(sourceCode)) {
        sourceCode.text += 3;
        sourceCode.size -= 3;
    }
    RcnCountResult lineCount = rcnCountPhysicalLines(sourceCode);
    if (!lineCount.state.ok) {
        return resultText;
    }

    NodeEvalContext* ctx = createNodeEvalContextAnnotation(
        language,
        lineCount.count
    );
    if (!ctx) {
        return resultText;
    }

    RcnCountResult result = {0};
    NodeEvalTrace trace = {0};
    trace.result = &result;
    trace.ctx = ctx;
    RcnResultState evalState = evaluateSourceTree(
        sourceCode,
        language,
        annotateLineWithNodeType,
        &trace
    );
    if (evalState.ok) {
        resultText = buildAnnotatedSource(sourceCode.text, &trace);
    }
    freeNodeEvalContextAnnotation(ctx);
    return resultText;
}
