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
#include <stddef.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "scount.h"
#include "reckon/reckon.h"

#define MAX_DIGITS_INT64 22ULL

static const size_t BUFFER_CAPACITY_INIT = 1024;
static const size_t LARGE_RESULT_THRESHOLD = 16;

static const int WIDTH_COL0 = 26; // File
static const int WIDTH_COL1 = 11; // LLC
static const int WIDTH_COL2 = 11; // PHL
static const int WIDTH_COL3 = 11; // WRD
static const int WIDTH_COL4 = 11; // CHR
static const int WIDTH_COL5 = 11; // SZE
static const int COLUMN_PADDING = 2;
static const char TABLE_BORDER_HORIZONTAL_NORMAL = '-';
static const char TABLE_BORDER_HORIZONTAL_EMPHASIS = '=';
static const char TABLE_BORDER_VERTICAL_NORMAL = '|';
static const char TABLE_BORDER_VERTICAL_EMPHASIS = '|';
static const char TABLE_BORDER_CORNER = 'o';
static const char* TABLE_PADDING_LEFT = "  ";
static const char errorMessage[] = "Error";

#ifdef _WIN32
static inline bool isPathSep(char character) {
    return character == '/' || character == '\\';
}
static inline bool hasDriveIdent(const char* path) {
    return (
        isalpha((unsigned char) path[0])
        && path[1] == ':'
        && isPathSep(path[2])
    );
}
#else
static inline bool isPathSep(char character) {
    return character == '/';
}
#endif

static char* pathBasename(const char* path) {
    if (!path) {
        return NULL;
    }
    size_t length = strlen(path);
    // Trim trailing separators, but not root
    while (length > 1 && isPathSep(path[length - 1])) {
#ifdef _WIN32
        if (length == 3 && hasDriveIdent(path)) {
            break; // Keep "C:\" intact
        }
#endif
        --length;
    }
    // Find start of basename
    size_t start = 0;
    for (size_t index = length; index > 0; --index) {
        if (isPathSep(path[index - 1])) {
            start = index;
            break;
        }
    }
    const size_t nameLength = length - start;
    char* name = malloc(nameLength + 1);
    if (!name) {
        return NULL;
    }
    memcpy(name, path + start, nameLength);
    name[nameLength] = '\0';
    return name;
}

static bool ensureCapacity(PrintBuffer* buffer, size_t additional) {
    const size_t requiredSize = buffer->size + additional + 1;
    if (requiredSize > buffer->capacity) {
        size_t newCapacity = (
            buffer->capacity
            ? buffer->capacity
            : BUFFER_CAPACITY_INIT
        );
        while (newCapacity < requiredSize) {
            assert(newCapacity != 0);
            if (newCapacity > (SIZE_MAX / 2)) {
                return false; // Overflow guard
            }
            newCapacity *= 2;
        }
        char* reallocatedData = realloc(buffer->text, newCapacity);
        if (!reallocatedData) {
            return false;
        }
        buffer->text = reallocatedData;
        buffer->capacity = newCapacity;
    }
    assert(buffer->text != NULL);
    return true;
}

/**
 * Puts a string up to `length` into the buffer.
 * The specified length must not exceed the actual length of `string`.
 */
static void prStrN(PrintBuffer* buffer, const char* string, size_t length) {
    if (!string || length == 0) {
        return;
    }
    assert(length <= strlen(string));
    if (!ensureCapacity(buffer, length)) {
        return;
    }
    memcpy(buffer->text + buffer->size, string, length);
    buffer->size += length;
    buffer->text[buffer->size] = '\0';
}

/**
 * Puts a string into the buffer.
 */
static void prStr(PrintBuffer* buffer, const char* string) {
    prStrN(buffer, string, (string != NULL ? strlen(string) : 0));
}

/**
 * Puts a single character into the buffer.
 */
static void prChr(PrintBuffer* buffer, char character) {
    if (!ensureCapacity(buffer, 1)) {
        return;
    }
    buffer->text[buffer->size++] = character;
    buffer->text[buffer->size] = '\0';
}

/**
 * Puts a long integer into the buffer.
 * The given number is formatted right‑aligned and possibly padded on
 * the left with spaces to fill the width of 8 characters.
 */
static void pr8ld(PrintBuffer* buffer, RcnCount value) {
    if (!ensureCapacity(buffer, MAX_DIGITS_INT64)) {
        return;
    }
    const int written = snprintf(
        buffer->text + buffer->size,
        MAX_DIGITS_INT64,
        "%8llu",
        (unsigned long long) value
    );
    buffer->size += (written > 0 ? (size_t) written : 0);
}

/**
 * Puts a `size_t` value into the buffer.
 */
static void prSize(PrintBuffer* buffer, size_t value) {
    if (!ensureCapacity(buffer, MAX_DIGITS_INT64)) {
        return;
    }
    const int written = snprintf(
        buffer->text + buffer->size,
        MAX_DIGITS_INT64,
        "%zu",
        value
    );
    buffer->size += (written > 0 ? (size_t)written : 0);
}

/**
 * Puts a specific character into the buffer repeatedly for `count` times.
 */
static void prRpt(PrintBuffer* buffer, char character, size_t count) {
    if (count == 0) {
        return;
    }
    if (!ensureCapacity(buffer, count)) {
        return;
    }
    memset(buffer->text + buffer->size, character, count);
    buffer->size += count;
    buffer->text[buffer->size] = '\0';
}

/**
 * Puts a `RcnCount` value into the buffer.
 * The given number is formatted centered within the specified width.
 */
static void prCnt(PrintBuffer* buffer, RcnCount value, int width) {
    width -= COLUMN_PADDING;
    char string[MAX_DIGITS_INT64];
    int written = snprintf(
        string,
        sizeof(string),
        "%llu",
        (unsigned long long) value
    );
    if (written < 0) {
        static_assert(sizeof(errorMessage) < sizeof(string), "Size mismatch");
        memcpy(string, errorMessage, sizeof(errorMessage));
        written = sizeof(errorMessage) - 1;
    } else if (written > width) {
        written = width;
    }
    const int padding = width - written;
    const int left = padding / 2;
    const int right = padding - left;
    prRpt(buffer, ' ', left);
    prStrN(buffer, string, (size_t) written);
    prRpt(buffer, ' ', right);
}

static void prHeaderCell(PrintBuffer* buffer, const char* label, int width) {
    assert(label != NULL);
    const int length = (int) strlen(label);
    int padding = width - (length + 2);
    assert(padding >= 0);
    const int left = padding / 2;
    const int right = padding - left;
    prRpt(buffer, '-', left);
    prChr(buffer, ' ');
    prStr(buffer, label);
    prChr(buffer, ' ');
    prRpt(buffer, '-', right);
}

static void prLeftEllipse(PrintBuffer* buffer, const char* text, int width) {
    width -= COLUMN_PADDING;
    const char* string = text ? text : "n/a";
    const int length = (int) strlen(string);
    if (length <= width) {
        prStr(buffer, string);
        prRpt(buffer, ' ', width - length);
    } else {
        prStr(buffer, "...");
        const int tail = width - 3;
        if (tail > 0) {
            prStrN(buffer, string + (length - tail), (size_t) tail);
        }
    }
}

static void prInputInfo(
    PrintBuffer* buffer,
    const char* path,
    const RcnCountStatistics* stats
) {
    char* name = pathBasename(path);
    prStr(buffer, "Directory: ");
    prStr(buffer, (name && name[0] != '\0') ? name : "(unknown name)");
    prChr(buffer, '\n');
    prStr(buffer, "Scanned files: ");
    prSize(buffer, stats->count.size);
    prChr(buffer, '\n');
    prChr(buffer, '\n');

    // Suppress false positive. This is really stupid.

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-double-free"
#endif
    free(name);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

}

static void prTableTop(PrintBuffer* buffer, const char* title) {
    prStr(buffer, TABLE_PADDING_LEFT);
    prChr(buffer, TABLE_BORDER_CORNER);
    prHeaderCell(buffer, title, WIDTH_COL0);
    prChr(buffer, TABLE_BORDER_CORNER);
    prHeaderCell(buffer, "LLC", WIDTH_COL1);
    prChr(buffer, TABLE_BORDER_CORNER);
    prHeaderCell(buffer, "PHL", WIDTH_COL2);
    prChr(buffer, TABLE_BORDER_CORNER);
    prHeaderCell(buffer, "WRD", WIDTH_COL3);
    prChr(buffer, TABLE_BORDER_CORNER);
    prHeaderCell(buffer, "CHR", WIDTH_COL4);
    prChr(buffer, TABLE_BORDER_CORNER);
    prHeaderCell(buffer, "SZE", WIDTH_COL5);
    prChr(buffer, TABLE_BORDER_CORNER);
    prChr(buffer, '\n');
}

static void prTableBottom(PrintBuffer* buffer, char border) {
    prStr(buffer, TABLE_PADDING_LEFT);
    prChr(buffer, TABLE_BORDER_CORNER);
    prRpt(buffer, border, WIDTH_COL0);
    prChr(buffer, TABLE_BORDER_CORNER);
    prRpt(buffer, border, WIDTH_COL1);
    prChr(buffer, TABLE_BORDER_CORNER);
    prRpt(buffer, border, WIDTH_COL2);
    prChr(buffer, TABLE_BORDER_CORNER);
    prRpt(buffer, border, WIDTH_COL3);
    prChr(buffer, TABLE_BORDER_CORNER);
    prRpt(buffer, border, WIDTH_COL4);
    prChr(buffer, TABLE_BORDER_CORNER);
    prRpt(buffer, border, WIDTH_COL5);
    prChr(buffer, TABLE_BORDER_CORNER);
    prChr(buffer, '\n');
}

static void prFileRowSkipped(PrintBuffer* buffer) {
    const char* ellipsisOdd = "...";
    const char* ellipsisEven = "..";
    const int ellOddLen = (int) strlen(ellipsisOdd);
    const int ellEvenLen = (int) strlen(ellipsisEven);
    prStr(buffer, TABLE_PADDING_LEFT);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    bool even = WIDTH_COL0 % 2 == 0;
    int correction = even ? ellEvenLen : ellOddLen;
    prRpt(buffer, ' ', (WIDTH_COL0 - correction) / 2);
    prStr(buffer, even ? ellipsisEven : ellipsisOdd);
    prRpt(buffer, ' ', (WIDTH_COL0 - correction) / 2);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    even = WIDTH_COL1 % 2 == 0;
    correction = even ? ellEvenLen : ellOddLen;
    prRpt(buffer, ' ', (WIDTH_COL1 - correction) / 2);
    prStr(buffer, even ? ellipsisEven : ellipsisOdd);
    prRpt(buffer, ' ', (WIDTH_COL1 - correction) / 2);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    even = WIDTH_COL2 % 2 == 0;
    correction = even ? ellEvenLen : ellOddLen;
    prRpt(buffer, ' ', (WIDTH_COL2 - correction) / 2);
    prStr(buffer, even ? ellipsisEven : ellipsisOdd);
    prRpt(buffer, ' ', (WIDTH_COL2 - correction) / 2);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    even = WIDTH_COL3 % 2 == 0;
    correction = even ? ellEvenLen : ellOddLen;
    prRpt(buffer, ' ', (WIDTH_COL3 - correction) / 2);
    prStr(buffer, even ? ellipsisEven : ellipsisOdd);
    prRpt(buffer, ' ', (WIDTH_COL3 - correction) / 2);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    even = WIDTH_COL4 % 2 == 0;
    correction = even ? ellEvenLen : ellOddLen;
    prRpt(buffer, ' ', (WIDTH_COL4 - correction) / 2);
    prStr(buffer, even ? ellipsisEven : ellipsisOdd);
    prRpt(buffer, ' ', (WIDTH_COL4 - correction) / 2);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    even = WIDTH_COL5 % 2 == 0;
    correction = even ? ellEvenLen : ellOddLen;
    prRpt(buffer, ' ', (WIDTH_COL5 - correction) / 2);
    prStr(buffer, even ? ellipsisEven : ellipsisOdd);
    prRpt(buffer, ' ', (WIDTH_COL5 - correction) / 2);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, '\n');
}

static void prFileRowData(
    PrintBuffer* buffer,
    const char* fileName,
    const RcnCountResultGroup* res
) {
    prStr(buffer, TABLE_PADDING_LEFT);
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prLeftEllipse(buffer, fileName, WIDTH_COL0);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, res->logicalLines, WIDTH_COL1);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, res->physicalLines, WIDTH_COL2);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, res->words, WIDTH_COL3);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, res->characters, WIDTH_COL4);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, res->sourceSize, WIDTH_COL5);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, '\n');
}

static void prFileRows(PrintBuffer* buffer, const RcnCountStatistics* stats) {
    const bool isLargeResult = stats->count.size > LARGE_RESULT_THRESHOLD;
    bool ellipsisRowPrinted = false;
    const size_t nFiles = stats->count.size;
    for (size_t i = 0; i < nFiles; ++i) {
        const RcnSourceFile* file = &stats->count.files[i];
        const RcnCountResultGroup* resultGroup = &stats->count.results[i];
        if (resultGroup->state.errorCode != RCN_ERR_NONE ){
            continue;
        }
        const bool isInSkipRange = (
            i >= LARGE_RESULT_THRESHOLD - 1
            && i != nFiles - 1
        );
        if (isLargeResult && isInSkipRange) {
            if (ellipsisRowPrinted) {
                continue;
            }
            prFileRowSkipped(buffer);
            ellipsisRowPrinted = true;
        } else {
            const char* fileName = (
                file && file->name
                ? file->name
                : "(unknown)"
            );
            prFileRowData(buffer, fileName, resultGroup);
        }
    }
}

static void prSummaryRows(
    PrintBuffer* buffer,
    const RcnCountStatistics* stats
) {
    for (RcnTextFormat frmt = 0; frmt < RECKON_NUM_SUPPORTED_FORMATS; ++frmt) {
        const char* label = NULL;
        switch (frmt) {
            case RCN_TEXT_UNFORMATTED:
                label = "Plain Text";
                break;
            case RCN_LANG_C:
                label = "C";
                break;
            case RCN_LANG_JAVA:
                label = "Java";
                break;
            // LCOV_EXCL_START
            default:
                assert(0 && "Unhandled text format");
                break;
            // LCOV_EXCL_STOP
        }
        assert(label != NULL);

        if (stats->sourceSize[frmt] == 0) {
            continue;
        }

        prStr(buffer, TABLE_PADDING_LEFT);
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, ' ');
        prLeftEllipse(buffer, label, WIDTH_COL0);
        prChr(buffer, ' ');
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, ' ');
        prCnt(buffer, stats->logicalLines[frmt], WIDTH_COL1);
        prChr(buffer, ' ');
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, ' ');
        prCnt(buffer, stats->physicalLines[frmt], WIDTH_COL2);
        prChr(buffer, ' ');
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, ' ');
        prCnt(buffer, stats->words[frmt], WIDTH_COL3);
        prChr(buffer, ' ');
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, ' ');
        prCnt(buffer, stats->characters[frmt], WIDTH_COL4);
        prChr(buffer, ' ');
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, ' ');
        prCnt(buffer, stats->sourceSize[frmt], WIDTH_COL5);
        prChr(buffer, ' ');
        prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
        prChr(buffer, '\n');
    }
}

static void prTotalsRow(PrintBuffer* buffer, const RcnCountStatistics* stats) {
    prStr(buffer, TABLE_PADDING_LEFT);
    prChr(buffer, TABLE_BORDER_VERTICAL_EMPHASIS);
    prChr(buffer, ' ');
    prLeftEllipse(buffer, "Total:", WIDTH_COL0);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, stats->totalLogicalLines, WIDTH_COL1);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, stats->totalPhysicalLines, WIDTH_COL2);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, stats->totalWords, WIDTH_COL3);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, stats->totalCharacters, WIDTH_COL4);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_NORMAL);
    prChr(buffer, ' ');
    prCnt(buffer, stats->totalSourceSize, WIDTH_COL5);
    prChr(buffer, ' ');
    prChr(buffer, TABLE_BORDER_VERTICAL_EMPHASIS);
    prChr(buffer, '\n');
}

PrintBuffer printResultSingle(const RcnCountStatistics* stats) {
    assert(stats != NULL);
    assert(stats->count.size == 1);
    PrintBuffer buffer = {0};
    RcnSourceFile* file = &stats->count.files[0];
    RcnCountResultGroup* result = &stats->count.results[0];

    prStr(&buffer, "File: ");
    prStr(&buffer, file->name ? file->name : "(unknown)");
    prChr(&buffer, '\n');
    prChr(&buffer, '\n');
    prStr(&buffer, "  Logical Lines of Code (LLC):   ");
    pr8ld(&buffer, result->logicalLines);
    prChr(&buffer, '\n');
    prStr(&buffer, "  Physical Lines        (PHL):   ");
    pr8ld(&buffer, result->physicalLines);
    prChr(&buffer, '\n');
    prStr(&buffer, "  Words                 (WRD):   ");
    pr8ld(&buffer, result->words);
    prChr(&buffer, '\n');
    prStr(&buffer, "  Characters            (CHR):   ");
    pr8ld(&buffer, result->characters);
    prChr(&buffer, '\n');
    prStr(&buffer, "  Source Size in Bytes  (SZE):   ");
    pr8ld(&buffer, result->sourceSize);
    prChr(&buffer, '\n');
    prChr(&buffer, '\n');

    return buffer;
}

PrintBuffer printResultsMultiple(
    const char* path,
    const RcnCountStatistics* stats
) {
    assert(path != NULL);
    assert(stats != NULL);
    assert(stats->count.size > 1);
    PrintBuffer buffer = {0};

    prInputInfo(&buffer, path, stats);

    prTableTop(&buffer, "File");
    prFileRows(&buffer, stats);
    prTableBottom(&buffer, TABLE_BORDER_HORIZONTAL_NORMAL);

    prStr(&buffer, "\nSummary:\n\n");

    prTableTop(&buffer, "Language");
    prSummaryRows(&buffer, stats);
    prTableBottom(&buffer, TABLE_BORDER_HORIZONTAL_EMPHASIS);
    prTotalsRow(&buffer, stats);
    prTableBottom(&buffer, TABLE_BORDER_HORIZONTAL_EMPHASIS);
    prStr(&buffer, "\n\n");

    return buffer;
}
