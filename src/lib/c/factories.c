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

#include <string.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "evaluation.h"
#include "fileio.h"

TSParser* createParserC(void);
TSParser* createParserJava(void);

void evaluateNodeC(TSNode node, NodeEvalTrace* trace);
void evaluateNodeJava(TSNode node, NodeEvalTrace* trace);

TSParser* createParser(RcnTextFormat language) {
    switch (language) {
        case RCN_LANG_C:
            return createParserC();
        case RCN_LANG_JAVA:
            return createParserJava();
        default:
            return NULL;
    }
}

NodeVisitor createEvaluationFunction(RcnTextFormat language) {
    switch (language) {
        case RCN_LANG_C:
            return evaluateNodeC;
        case RCN_LANG_JAVA:
            return evaluateNodeJava;
        default:
            return NULL;
    }
}

const char* getInlineSourceCommentString(RcnTextFormat language) {
    switch (language) {
        case RCN_LANG_C:
        case RCN_LANG_JAVA:
        default:
            return "//";
    }
}

SourceFormatDetection detectSourceFormat(const RcnSourceFile* file) {
    SourceFormatDetection detection = {
        .isSupportedFormat = false,
        .isProgrammingLanguage = false,
        .format = RCN_TEXT_UNFORMATTED // undefined placeholder
    };

    if (!file || !file->extension) {
        return detection;
    }

    const char* extension = file->extension;
    if (strcmp(extension, "c") == 0 || strcmp(extension, "h") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_C;
    } else if (strcmp(extension, "java") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_JAVA;
    } else if (strcmp(extension, "md") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_MARKDOWN;
    } else if (strcmp(extension, "txt") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_UNFORMATTED;
    }

    return detection;
}
