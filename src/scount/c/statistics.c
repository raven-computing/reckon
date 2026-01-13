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
#include <stddef.h>
#include <string.h>

#include "reckon/reckon.h"
#include "scount.h"

static void reportError(const char* path, RcnCountStatistics* stats) {
    if (stats->state.errorCode == RCN_ERR_INVALID_INPUT) {
        logE("Invalid input path: '%s'", path);
    } else {
        logE("An error has occurred for: '%s'", path);
    }
    if (stats->state.errorMessage) {
        logE(
            "%s (%#04x)",
            stats->state.errorMessage,
            stats->state.errorCode
        );
    } else {
        // LCOV_EXCL_START
        logE(
            "An unknown error has occurred. Error code: %#04x",
            stats->state.errorCode
        );
        // LCOV_EXCL_STOP
    }
}

static void reportInputVerbose(const char* path, RcnCountStatistics* stats) {
    logV("Processing input path: '%s'", path);
    const char* const fileLabel = stats->count.size == 1 ? "file" : "files";
    logV("A total of %zu %s found", stats->count.size, fileLabel);
    for (size_t i = 0; i < stats->count.size; ++i) {
        RcnSourceFile* file = &stats->count.files[i];
        logV(
            "Found file: '%s' (status: 0x%#04x)",
            file->path,
            file->status
        );
    }
}

static void reportNothingWasProc(const char* path, RcnCountStatistics* stats) {
    if (stats->count.size == 1) {
        const RcnSourceFile* const file = &stats->count.files[0];
        const bool inputIsDirectory = strcmp(path, file->path) != 0;
        logE("Scanned %s '%s'", inputIsDirectory ? "directory" : "file", path);
        logE("The file '%s' cannot be processed.", file->name);
        logE(
            "It does not have an eligible format.%s%s%s%s",
            file->extension ? " The file extension is not supported: " : "",
            file->extension ? "'" : "",
            file->extension ? file->extension : "",
            file->extension ? "'" : ""
        );
    } else {
        logE("Scanned directory '%s'", path);
        logE("It contains no files with an eligible format.");
    }
}

ExitStatus outputStatistics(AppArgs args) {
    const char* const path = args.inputPath;
    if (!path) {
        return APP_EXIT_INVALID_INPUT;
    }
    RcnCountStatistics* const stats = rcnCreateCountStatistics(path);
    if(!stats) {
        // LCOV_EXCL_START
        logE("Failed to create count statistics for path: '%s'", path);
        return APP_EXIT_INVALID_INPUT;
        // LCOV_EXCL_STOP
    }
    if(stats->state.errorCode != RCN_ERR_NONE) {
        reportError(path, stats);
        rcnFreeCountStatistics(stats);
        return APP_EXIT_INVALID_INPUT;
    }
    if (LOG_LEVEL >= LOG_LEVEL_VERBOSE) {
        reportInputVerbose(path, stats);
    }

    RcnStatOptions options = {0};
    rcnCount(stats, options);

    const RcnErrorCode errorCode = stats->state.errorCode;
    if (!stats->state.ok && errorCode != RCN_ERR_UNSUPPORTED_FORMAT) {
        reportError(path, stats);
        rcnFreeCountStatistics(stats);
        return APP_EXIT_INVALID_INPUT;
    }

    if (stats->count.sizeProcessed == 0) {
        reportNothingWasProc(path, stats);
        rcnFreeCountStatistics(stats);
        return APP_EXIT_NOTHING_PROCESSED;
    }

    PrintBuffer buffer = (
        stats->count.size == 1
        ? printResultSingle(stats)
        : printResultsMultiple(path, stats)
    );

    if (buffer.size > 0) {
        logStdout(buffer.text);
    }
    free(buffer.text);

    rcnFreeCountStatistics(stats);
    return APP_EXIT_SUCCESS;
}
