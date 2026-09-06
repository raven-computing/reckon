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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "reckon/reckon.h"
#include "evaluation.h"
#include "fileio.h"
#include "threading.h"

/**
 * Asserts that the given RcnTextFormat enumerator value is within bounds.
 */
#define ASSERT_SOURCE_FORMAT_INDEX(sourceFormat) \
    assert((sourceFormat) < RECKON_NUM_SUPPORTED_FORMATS && \
"RcnTextFormat enumerator value out of bounds. \
Did you forget to update RECKON_NUM_SUPPORTED_FORMATS \
after adding support for another text format?" \
);

/**
 * If all option bits are zero, semantically, all ops/formats are
 * selected so in that case all bits are explicitly set to ones so
 * that subsequently that case does not need to be checked anymore.
 */
static const uint32_t DEFAULT_OPT_ENABLE_ALL = 0xffffffff;

static_assert(RECKON_NUM_SUPPORTED_FORMATS < 64, "Too many formats");

static size_t getWorkerCount(size_t fileCount, RcnStatOptions options) {
    if (!options.useMultiThreading || fileCount < 2) {
        return 1;
    }
    size_t workerCount = getSystemConcurrency();
    if (workerCount < 2) {
        return 1;
    }
    if (workerCount > fileCount) {
        workerCount = fileCount;
    }
    return workerCount;
}

static bool isFormatSelected(RcnStatOptions options, RcnTextFormat srcFormat) {
    return (options.formats & RECKON_MK_FRMT_OPT(srcFormat)) != 0;
}

static inline void resetResultGroup(RcnCountResultGroup* resultGroup) {
    resultGroup->logicalLines = 0;
    resultGroup->codeLines = 0;
    resultGroup->physicalLines = 0;
    resultGroup->words = 0;
    resultGroup->characters = 0;
    resultGroup->sourceSize = 0;
    resultGroup->state.ok = false;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    resultGroup->state.errorMessage = NULL;
    resultGroup->isProcessed = false;
    resultGroup->hasLogicalLines = false;
    resultGroup->hasCodeLines = false;
}

static inline bool hasCriticalError(RcnResultState state) {
    const RcnErrorCode errorCode = state.errorCode;
    return errorCode == RCN_ERR_ALLOC_FAILURE || errorCode == RCN_ERR_UNKNOWN;
}

static void resetAggregateStatistics(RcnCountStatistics* stats) {
    stats->totalLogicalLines = 0;
    stats->totalCodeLines = 0;
    stats->totalPhysicalLines = 0;
    stats->totalWords = 0;
    stats->totalCharacters = 0;
    stats->totalSourceSize = 0;
    stats->count.sizeProcessed = 0;
    for (RcnTextFormat frmt = 0; frmt < RECKON_NUM_SUPPORTED_FORMATS; ++frmt) {
        stats->logicalLines[frmt] = 0;
        stats->codeLines[frmt] = 0;
        stats->physicalLines[frmt] = 0;
        stats->words[frmt] = 0;
        stats->characters[frmt] = 0;
        stats->sourceSize[frmt] = 0;
    }
}

static void updateStatsState(
    RcnResultState* state,
    RcnResultState resultState,
    bool stopOnError
) {
    if (resultState.errorCode == RCN_ERR_NONE) {
        return;
    }
    if (resultState.errorCode == RCN_ERR_UNSUPPORTED_FORMAT) {
        return;
    }
    state->errorCode = resultState.errorCode;
    state->errorMessage = resultState.errorMessage;
    if (stopOnError || hasCriticalError(resultState)) {
        state->ok = false;
    }
}

static void aggregateStatistics(
    RcnCountStatistics* stats,
    RcnStatOptions options
) {
    if (!stats->state.ok) {
        return;
    }
    resetAggregateStatistics(stats);
    stats->state.ok = true;
    stats->state.errorCode = RCN_ERR_NONE;
    stats->state.errorMessage = NULL;

    for (size_t i = 0; i < stats->count.size; ++i) {
        RcnSourceFile* file = &stats->count.files[i];
        RcnCountResultGroup* result = &stats->count.results[i];
        if (result->isProcessed) {
            SourceFormatDetection detected = detectSourceFormat(file);
            if (detected.isSupportedFormat) {
                const RcnTextFormat sourceFormat = detected.format;
                ASSERT_SOURCE_FORMAT_INDEX(sourceFormat);
                stats->totalLogicalLines += result->logicalLines;
                stats->totalCodeLines += result->codeLines;
                stats->totalPhysicalLines += result->physicalLines;
                stats->totalWords += result->words;
                stats->totalCharacters += result->characters;
                stats->totalSourceSize += result->sourceSize;
                stats->logicalLines[sourceFormat] += result->logicalLines;
                stats->codeLines[sourceFormat] += result->codeLines;
                stats->physicalLines[sourceFormat] += result->physicalLines;
                stats->words[sourceFormat] += result->words;
                stats->characters[sourceFormat] += result->characters;
                stats->sourceSize[sourceFormat] += result->sourceSize;
                stats->count.sizeProcessed += 1;
            }
        }
        updateStatsState(&stats->state, result->state, options.stopOnError);
    }
}

static inline bool ensureFileContent(
    RcnSourceFile* file,
    RcnCountResultGroup* resultGroup
) {
    if (!file->isContentRead) {
        if (!readSourceFileContent(file)) {
            resultGroup->state.errorCode = RCN_ERR_INVALID_INPUT;
            resultGroup->state.errorMessage = "Failed to read file content";
            resultGroup->state.ok = false;
            return false;
        }
    }
    if (file->status != RCN_FILE_OP_OK || !file->content.text) {
        resultGroup->state.errorCode = RCN_ERR_INVALID_INPUT;
        resultGroup->state.errorMessage = "Failed to read file content";
        resultGroup->state.ok = false;
        return false;
    }
    return true;
}

static bool checkIntermediateResultState(
    RcnCountResultGroup* resultGroup,
    RcnResultState state
) {
    switch (state.errorCode) {
        case RCN_ERR_NONE:
            return true;
        default:
            resultGroup->logicalLines = 0;
            resultGroup->codeLines = 0;
            resultGroup->physicalLines = 0;
            resultGroup->words = 0;
            resultGroup->characters = 0;
            resultGroup->sourceSize = 0;
            resultGroup->isProcessed = false;
            resultGroup->state = state;
        }
        return false;
}

static inline bool countLogicalLines(
    RcnStatOptions options,
    RcnSourceFile* file,
    RcnTextFormat language,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result =
        options.strict
        ? rcnCountLogicalLinesStrict(language, file->content)
        : rcnCountLogicalLines(language, file->content);

    if (!checkIntermediateResultState(resultGroup, result.state)) {
        return false;
    }
    resultGroup->logicalLines = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    return true;
}

static inline bool countCodeLines(
    RcnSourceFile* file,
    RcnTextFormat language,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountLinesOfCode(language, file->content);
    if (!checkIntermediateResultState(resultGroup, result.state)) {
        return false;
    }
    resultGroup->codeLines = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    return true;
}

static inline bool countPhysicalLines(
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    if (!checkIntermediateResultState(resultGroup, result.state)) {
        return false;
    }
    resultGroup->physicalLines = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    return true;
}

static inline bool countWords(
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountWords(file->content);
    if (!checkIntermediateResultState(resultGroup, result.state)) {
        return false;
    }
    resultGroup->words = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    return true;
}

static inline bool countCharacters(
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountCharacters(file->content);
    if (!checkIntermediateResultState(resultGroup, result.state)) {
        return false;
    }
    resultGroup->characters = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    return true;
}

static inline void countProcessedFile(
    RcnSourceFile* file,
    RcnCountResultGroup* resultGroup
) {
    const RcnCount fileSize = file->content.size;
    resultGroup->isProcessed = true;
    resultGroup->sourceSize = fileSize;
}

static bool collectFiles(const char* directory, RcnCountStatistics* stats) {
    SourceFileList list = newSourceFileList(directory);
    if (!list.ok) {
        return false;
    }
    RcnCountResultGroup* groups = NULL;
    if (list.size > 0) {
        groups = calloc(
            list.size,
            sizeof(RcnCountResultGroup)
        );
        if (!groups) {
            freeSourceFileList(&list);
            return false;
        }
    }
    stats->count.results = groups;
    stats->count.files = list.files; // Ownership transfer
    stats->count.size = list.size;
    return true;
}

static bool setupFile(const char* regularFile, RcnCountStatistics* stats) {
    RcnSourceFile* file = newSourceFile(regularFile);
    if (file) {
        RcnCountResultGroup* group = calloc(1, sizeof(RcnCountResultGroup));
        if (group) {
            stats->count.results = group;
            stats->count.files = file;
            stats->count.size = 1;
            return true;
        }
        freeSourceFile(file);
    }
    return false;
}

static inline bool count(
    RcnStatOptions options,
    RcnSourceFile* file,
    RcnCountResultGroup* result,
    SourceFormatDetection detected
) {
    RCN_LOG_DBG("Processing file:")
    RCN_LOG_DBG(file->path)

    bool ok = false;
    result->hasLogicalLines = rcnIsLlcCountingSupported(detected.format);
    result->hasCodeLines = rcnIsLocCountingSupported(detected.format);
    RcnTextFormat sourceFormat = detected.format;
    ok = ensureFileContent(file, result);
    if (ok && options.operations & RCN_OPT_COUNT_LOGICAL_LINES){
        if (result->hasLogicalLines) {
            ok = countLogicalLines(options, file, sourceFormat, result);
        }
    }
    if (ok && options.operations & RCN_OPT_COUNT_CODE_LINES) {
        if (result->hasCodeLines) {
            ok = countCodeLines(file, sourceFormat, result);
        }
    }
    if (ok && options.operations & RCN_OPT_COUNT_PHYSICAL_LINES) {
        ok = countPhysicalLines(file, sourceFormat, result);
    }
    if (ok && options.operations & RCN_OPT_COUNT_WORDS) {
        ok = countWords(file, sourceFormat, result);
    }
    if (ok && options.operations & RCN_OPT_COUNT_CHARACTERS) {
        ok = countCharacters(file, sourceFormat, result);
    }
    if (ok) {
        countProcessedFile(file, result);
    }
    if (!options.keepFileContent) {
        freeSourceFileContent(file);
    }

    RCN_LOG_DBG("Done processing file:")
    RCN_LOG_DBG(file->path)
    return ok;
}

static void processFileRange(
    RcnCountStatistics* stats,
    RcnStatOptions options,
    Slice slice,
    ThreadControl* control
) {
    for (size_t i = slice.start; i < slice.end; ++i) {
        if (shouldAbortRange(control)) {
            break;
        }

        RcnSourceFile* file = &stats->count.files[i];
        RcnCountResultGroup* result = &stats->count.results[i];
        resetResultGroup(result);

        SourceFormatDetection detected = detectSourceFormat(file);
        if (!detected.isSupportedFormat) {
            result->state.errorCode = RCN_ERR_UNSUPPORTED_FORMAT;
            result->state.errorMessage = "The source format is not supported";
            continue;
        }

        RcnTextFormat sourceFormat = detected.format;
        ASSERT_SOURCE_FORMAT_INDEX(sourceFormat);
        if (!isFormatSelected(options, sourceFormat)) {
            continue;
        }

        const bool ok = count(options, file, result, detected);
        if (!ok) {
            if (options.stopOnError || hasCriticalError(result->state)) {
                requestAbortRange(control);
                break;
            }
        }
    }
}

static void runCountThread(ThreadWork* arg) {
    processFileRange(
        arg->stats,
        arg->options,
        arg->slice,
        arg->control
    );
}

static bool parallelizeCount(
    RcnCountStatistics* stats,
    RcnStatOptions options,
    size_t workerCount
) {
    ThreadHandle* threads = calloc(workerCount, sizeof(ThreadHandle));
    ThreadWork* workItems = calloc(workerCount, sizeof(ThreadWork));
    if (!threads || !workItems) {
        free(threads);
        free(workItems);
        return false;
    }

    ThreadControl control;
    if (!initThreadControl(&control)) {
        free(threads);
        free(workItems);
        return false;
    }

    const size_t baseChunkSize = stats->count.size / workerCount;
    const size_t remainder = stats->count.size % workerCount;
    size_t startIndex = 0;
    size_t createdThreads = 0;
    bool createdAllThreads = true;

    for (size_t i = 0; i < workerCount; ++i) {
        const size_t chunkSize = baseChunkSize + (i < remainder ? 1 : 0);
        ThreadWork* chunk = &workItems[i];
        chunk->stats = stats;
        chunk->options = options;
        chunk->slice = (Slice){
            .start = startIndex,
            .end = startIndex + chunkSize
        };
        chunk->control = &control;
        startIndex = chunk->slice.end;

        if (!createThread(&threads[i], runCountThread, chunk)) {
            createdAllThreads = false;
            break;
        }
        createdThreads += 1;
    }

    if (!createdAllThreads) {
        requestAbortRange(&control);
    }

    for (size_t i = 0; i < createdThreads; ++i) {
        joinThread(&threads[i]);
    }

    deinitThreadControl(&control);
    free(threads);
    free(workItems);
    return createdAllThreads;
}

RcnCountStatistics* rcnCreateCountStatistics(const char* path) {
    if (!path) {
        return NULL;
    }
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    if (!stats) {
        return NULL;
    }
    const char* errorMessage = isValidStatsInput(path);
    if (errorMessage) {
        stats->state.errorCode = RCN_ERR_INVALID_INPUT;
        stats->state.errorMessage = errorMessage;
        return stats;
    }
    const bool ok = (
        isDirectory(path)
        ? collectFiles(path, stats)
        : setupFile(path, stats)
    );
    if (!ok) {
        free(stats);
        return NULL;
    }
    return stats;
}

RcnCountStatistics* rcnAllocCountStatistics(size_t size) {
    RcnCountStatistics* stats = calloc(1, sizeof(RcnCountStatistics));
    if (!stats) {
        return NULL;
    }
    if (size > 0) {
        RcnSourceFile* files = calloc(size, sizeof(RcnSourceFile));
        if (!files) {
            free(stats);
            return NULL;
        }
        RcnCountResultGroup* groups = calloc(
            size,
            sizeof(RcnCountResultGroup)
        );
        if (!groups) {
            free(files);
            free(stats);
            return NULL;
        }
        stats->count.files = files;
        stats->count.results = groups;
        stats->count.size = size;
    }
    return stats;
}

void rcnFreeCountStatistics(RcnCountStatistics* stats) {
    if (stats) {
        if (stats->count.files) {
            SourceFileList list = {
                .files = stats->count.files,
                .size = stats->count.size,
                .capacity = stats->count.size
            };
            freeSourceFileList(&list);
            stats->count.size = 0;
            stats->count.files = NULL;
        }
        if (stats->count.results) {
            free(stats->count.results);
            stats->count.results = NULL;
        }
        free(stats);
    }
}

void rcnFreeSourceText(RcnSourceText* source) {
    if (source) {
        if (source->text) {
            free(source->text);
            source->text = NULL;
            source->size = 0;
        }
    }
}

void rcnCount(RcnCountStatistics* stats, RcnStatOptions options) {
    if (!stats) {
        return;
    }
    if (!stats->count.files) {
        stats->state.ok = false;
        stats->state.errorCode = RCN_ERR_INVALID_INPUT;
        stats->state.errorMessage = "No input files provided";
        return;
    }
    if (!stats->count.results) {
        stats->state.ok = false;
        stats->state.errorCode = RCN_ERR_INVALID_INPUT;
        stats->state.errorMessage = "No result list provided";
        return;
    }

    if (options.operations == 0) {
        options.operations = DEFAULT_OPT_ENABLE_ALL;
    }
    if (options.formats == 0) {
        options.formats = DEFAULT_OPT_ENABLE_ALL;
    }

    // Set as successful upfront, is potentially invalidated inside loop
    stats->state.ok = true;
    stats->state.errorCode = RCN_ERR_NONE;
    stats->state.errorMessage = NULL;

    const size_t workerCount = getWorkerCount(stats->count.size, options);
    if (workerCount > 1) {
        const bool ok = parallelizeCount(stats, options, workerCount);
        if (!ok) {
            stats->state.ok = false;
            stats->state.errorCode = RCN_ERR_UNKNOWN;
            stats->state.errorMessage = "Failed to run in parallel";
        }
    } else {
        processFileRange(
            stats,
            options,
            (Slice){0, stats->count.size},
            NULL
        );
    }
    aggregateStatistics(stats, options);
    if (stats->count.size == 1) {
        stats->state = stats->count.results[0].state;
    }
}
