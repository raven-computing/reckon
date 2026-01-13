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

#include "reckon/reckon.h"
#include "scount.h"

ExitStatus outputAnnotatedSource(AppArgs args) {
    RcnSourceText annotatedSource = rcnMarkLogicalLinesInFile(args.inputPath);
    if (!annotatedSource.text) {
        logE("Failed to annotate source file '%s'", args.inputPath);
        logE(
            "Check the logical line count of that file to try to "
            "get more information on the error."
        );
        logE(
            "Hint: Try to run the previous command "
            "without the '--annotate-counts' option."
        );
        return APP_EXIT_INVALID_INPUT;
    }
    logStdout(annotatedSource.text);
    rcnFreeSourceText(&annotatedSource);
    return APP_EXIT_SUCCESS;
}
