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
TSParser* createParserPython(void);
TSParser* createParserJavaScript(void);
TSParser* createParserTypeScript(void);
TSParser* createParserBash(void);
TSParser* createParserCpp(void);

void evaluateNodeC(TSNode node, NodeEvalTrace* trace);
void evaluateNodeJava(TSNode node, NodeEvalTrace* trace);
void evaluateNodePython(TSNode node, NodeEvalTrace* trace);
void evaluateNodeJavaScript(TSNode node, NodeEvalTrace* trace);
void evaluateNodeTypeScript(TSNode node, NodeEvalTrace* trace);
void evaluateNodeBash(TSNode node, NodeEvalTrace* trace);
void evaluateNodeCpp(TSNode node, NodeEvalTrace* trace);

TSParser* createParser(RcnTextFormat language) {
    switch (language) {
        case RCN_LANG_C:
            return createParserC();
        case RCN_LANG_JAVA:
            return createParserJava();
        case RCN_LANG_PYTHON:
            return createParserPython();
        case RCN_LANG_JAVASCRIPT:
            return createParserJavaScript();
        case RCN_LANG_TYPESCRIPT:
            return createParserTypeScript();
        case RCN_LANG_BASH:
            return createParserBash();
        case RCN_LANG_CPP:
            return createParserCpp();
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
        case RCN_LANG_PYTHON:
            return evaluateNodePython;
        case RCN_LANG_JAVASCRIPT:
            return evaluateNodeJavaScript;
        case RCN_LANG_TYPESCRIPT:
            return evaluateNodeTypeScript;
        case RCN_LANG_BASH:
            return evaluateNodeBash;
        case RCN_LANG_CPP:
            return evaluateNodeCpp;
        default:
            return NULL;
    }
}

const char* getInlineSourceCommentString(RcnTextFormat language) {
    switch (language) {
        case RCN_LANG_PYTHON:
        case RCN_LANG_BASH:
            return "#";
        case RCN_LANG_C:
        case RCN_LANG_JAVA:
        case RCN_LANG_CPP:
        case RCN_LANG_JAVASCRIPT:
        case RCN_LANG_TYPESCRIPT:
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
    } else if (strcmp(extension, "py") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_PYTHON;
    } else if (strcmp(extension, "js") == 0
        || strcmp(extension, "mjs") == 0
        || strcmp(extension, "cjs") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_JAVASCRIPT;
    } else if (strcmp(extension, "ts") == 0
        || strcmp(extension, "mts") == 0
        || strcmp(extension, "cts") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_TYPESCRIPT;
    } else if (strcmp(extension, "sh") == 0
        || strcmp(extension, "bash") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_BASH;
    } else if (strcmp(extension, "cpp") == 0
        || strcmp(extension, "cc") == 0
        || strcmp(extension, "cxx") == 0
        || strcmp(extension, "c++") == 0
        || strcmp(extension, "hpp") == 0
        || strcmp(extension, "hxx") == 0
        || strcmp(extension, "cppm") == 0
        || strcmp(extension, "cxxm") == 0
        || strcmp(extension, "c++m") == 0
        || strcmp(extension, "ixx") == 0) {
        detection.isSupportedFormat = true;
        detection.isProgrammingLanguage = true;
        detection.format = RCN_LANG_CPP;
    } else if (strcmp(extension, "md") == 0
        || strcmp(extension, "markdown") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_MARKDOWN;
    } else if (strcmp(extension, "xml") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_XML;
    } else if (strcmp(extension, "json") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_JSON;
    } else if (strcmp(extension, "css") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_CSS;
    } else if (strcmp(extension, "html") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_HTML;
    } else if (strcmp(extension, "sql") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_SQL;
    } else if (strcmp(extension, "cmake") == 0
        || strcmp(file->name, "CMakeLists.txt") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_CMAKE;
    } else if (strcmp(extension, "yaml") == 0
        || strcmp(extension, "yml") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_YAML;
    } else if (strcmp(extension, "R") == 0
        || strcmp(extension, "r") == 0) {
        // isProgrammingLanguage is false for R format due to missing
        // support for logical line counting in R files
        detection.isSupportedFormat = true;
        detection.format = RCN_LANG_R;
    } else if (strcmp(extension, "txt") == 0) {
        detection.isSupportedFormat = true;
        detection.format = RCN_TEXT_UNFORMATTED;
    }

    return detection;
}
