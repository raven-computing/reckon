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

/**
 * Control flow macro used in the main processing loop in rcnCount().
 * Handles errors in a file processing interation. Either breaks out
 * of the loop or continues to the next iteration.
 * 
 * @param options The RcnStatOptions used in the count operation.
*/
#define LOOP_ITER_ERR(options) if ((options).stopOnError) break; else continue;

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

static bool isFormatSelected(RcnStatOptions options, RcnTextFormat srcFormat) {
    return (options.formats & RECKON_MK_FRMT_OPT(srcFormat)) != 0;
}

static inline void resetResultGroup(RcnCountResultGroup* resultGroup) {
    resultGroup->logicalLines = 0;
    resultGroup->physicalLines = 0;
    resultGroup->words = 0;
    resultGroup->characters = 0;
    resultGroup->sourceSize = 0;
    resultGroup->state.ok = false;
    resultGroup->isProcessed = false;
}

static inline bool ensureFileContent(
    RcnCountStatistics* stats,
    RcnStatOptions options,
    RcnSourceFile* file,
    RcnCountResultGroup* resultGroup
) {
    if (!file->isContentRead) {
        if (!readSourceFileContent(file)) {
            resultGroup->state.errorCode = RCN_ERR_INVALID_INPUT;
            resultGroup->state.errorMessage = "Failed to read file content";
            resultGroup->state.ok = false;
            stats->state.errorCode = RCN_ERR_INVALID_INPUT;
            stats->state.errorMessage = "Failed to read file content";
            if (options.stopOnError) {
                stats->state.ok = false;
            }
            return false;
        }
    }
    if (file->status != RCN_FILE_OP_OK || !file->content.text) {
        resultGroup->state.errorCode = RCN_ERR_INVALID_INPUT;
        resultGroup->state.ok = false;
        stats->state.errorCode = RCN_ERR_INVALID_INPUT;
        if (options.stopOnError) {
            stats->state.ok = false;
        }
        return false;
    }
    return true;
}

static bool checkIntermediateResultState(
    RcnCountStatistics* stats,
    RcnCountResultGroup* resultGroup,
    RcnResultState state
) {
    switch (state.errorCode) {
        case RCN_ERR_NONE:
            return true;
        case RCN_ERR_ALLOC_FAILURE:
        case RCN_ERR_UNKNOWN:
            stats->state.ok = false;
            stats->state.errorCode = state.errorCode;
            stats->state.errorMessage = state.errorMessage;
            FALLTHROUGH;
        default:
            resultGroup->state = state;
        }
        resetResultGroup(resultGroup);
        return false;
}

static inline bool countLogicalLines(
    RcnCountStatistics* stats,
    RcnSourceFile* file,
    RcnTextFormat language,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountLogicalLines(language, file->content);
    if (!checkIntermediateResultState(stats, resultGroup, result.state)) {
        return false;
    }
    resultGroup->logicalLines = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    stats->totalLogicalLines += resultGroup->logicalLines;
    stats->logicalLines[language] += resultGroup->logicalLines;
    return true;
}

static inline bool countPhysicalLines(
    RcnCountStatistics* stats,
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountPhysicalLines(file->content);
    if (!checkIntermediateResultState(stats, resultGroup, result.state)) {
        return false;
    }
    resultGroup->physicalLines = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    stats->totalPhysicalLines += resultGroup->physicalLines;
    stats->physicalLines[sourceFormat] += resultGroup->physicalLines;
    return true;
}

static inline bool countWords(
    RcnCountStatistics* stats,
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountWords(file->content);
    if (!checkIntermediateResultState(stats, resultGroup, result.state)) {
        return false;
    }
    resultGroup->words = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    stats->totalWords += resultGroup->words;
    stats->words[sourceFormat] += resultGroup->words;
    return true;
}

static inline bool countCharacters(
    RcnCountStatistics* stats,
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    RcnCountResult result = rcnCountCharacters(file->content);
    if (!checkIntermediateResultState(stats, resultGroup, result.state)) {
        return false;
    }
    resultGroup->characters = result.count;
    resultGroup->state.ok = true;
    resultGroup->state.errorCode = RCN_ERR_NONE;
    stats->totalCharacters += result.count;
    stats->characters[sourceFormat] += result.count;
    return true;
}

static inline void countProcessedFile(
    RcnCountStatistics* stats,
    RcnSourceFile* file,
    RcnTextFormat sourceFormat,
    RcnCountResultGroup* resultGroup
) {
    const RcnCount fileSize = file->content.size;
    resultGroup->isProcessed = true;
    resultGroup->sourceSize = fileSize;
    stats->count.sizeProcessed += 1;
    stats->totalSourceSize += fileSize;
    stats->sourceSize[sourceFormat] += fileSize;
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
    RcnCountStatistics* stats,
    RcnStatOptions options,
    RcnSourceFile* file,
    RcnCountResultGroup* result,
    SourceFormatDetection detected
) {
    RCN_LOG_DBG("Processing file:")
    RCN_LOG_DBG(file->path)

    bool ok = false;
    RcnTextFormat sourceFormat = detected.format;
    ok = ensureFileContent(stats, options, file, result);
    if (ok && options.operations & RCN_OPT_COUNT_LOGICAL_LINES){
        if (detected.isProgrammingLanguage) {
            ok = countLogicalLines(stats, file, sourceFormat, result);
        }
    }
    if (ok && options.operations & RCN_OPT_COUNT_PHYSICAL_LINES) {
        ok = countPhysicalLines(stats, file, sourceFormat, result);
    }
    if (ok && options.operations & RCN_OPT_COUNT_WORDS) {
        ok = countWords(stats, file, sourceFormat, result);
    }
    if (ok && options.operations & RCN_OPT_COUNT_CHARACTERS) {
        ok = countCharacters(stats, file, sourceFormat, result);
    }
    if (ok) {
        countProcessedFile(stats, file, sourceFormat, result);
    }
    if (!options.keepFileContent) {
        freeSourceFileContent(file);
    }
    if (!ok && options.stopOnError) {
        stats->state = result->state;
        stats->state.ok = false;
    }

    RCN_LOG_DBG("Done processing file:")
    RCN_LOG_DBG(file->path)
    return ok;
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

    for (size_t i = 0; i < stats->count.size; ++i) {
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
        const bool ok = count(stats, options, file, result, detected);
        if (!ok && (options.stopOnError || !stats->state.ok)) {
            break;
        }
    }
    if (stats->count.size == 1) {
        stats->state = stats->count.results[0].state;
    }
}
