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

#include "reckon/reckon.h"
#include "scount.h"

static RcnTextFormat formatFromExtension(const char* ext) {
    if (!ext) {
        return RCN_TEXT_UNFORMATTED;
    }
    if (strcmp(ext, "c") == 0 || strcmp(ext, "h") == 0) {
        return RCN_LANG_C;
    }
    if (strcmp(ext, "java") == 0) {
        return RCN_LANG_JAVA;
    }
    if (strcmp(ext, "py") == 0) {
        return RCN_LANG_PYTHON;
    }
    if (strcmp(ext, "js") == 0
            || strcmp(ext, "mjs") == 0
            || strcmp(ext, "cjs") == 0) {
        return RCN_LANG_JAVASCRIPT;
    }
    if (strcmp(ext, "ts") == 0) {
        return RCN_LANG_TYPESCRIPT;
    }
    if (strcmp(ext, "r") == 0 || strcmp(ext, "R") == 0) {
        return RCN_LANG_R;
    }
    if (strcmp(ext, "sh") == 0) {
        return RCN_LANG_BASH;
    }
    return RCN_TEXT_UNFORMATTED;
}

ExitStatus outputAnnotatedSource(AppArgs args) {
    RcnSourceText annotatedSource = {0};
    if (args.readFromStdin) {
        RcnCountStatistics* stats = readStdinToCountStatistics(args.inputPath);
        if (!stats) {
            logE("Failed to read source input from stdin.");
            return APP_EXIT_PROG_IO_ERROR;
        }
        const RcnSourceFile* const file = &stats->count.files[0];
        const RcnTextFormat format = formatFromExtension(file->extension);
        annotatedSource = rcnMarkLogicalLinesInSourceText(
            format,
            file->content
        );
        rcnFreeCountStatistics(stats);
    } else {
        annotatedSource = rcnMarkLogicalLinesInFile(args.inputPath);
    }
    if (!annotatedSource.text) {
        if (args.readFromStdin) {
            logE("Failed to annotate source input from stdin.");
        } else {
            logE("Failed to annotate source file '%s'", args.inputPath);
            logE(
                "Check the logical line count of that file to try to "
                "get more information on the error."
            );
            logE(
                "Hint: Try to run the previous command "
                "without the '--annotate-counts' option."
            );
        }
        return APP_EXIT_INVALID_INPUT;
    }
    logStdout(annotatedSource.text);
    rcnFreeSourceText(&annotatedSource);
    return APP_EXIT_SUCCESS;
}
