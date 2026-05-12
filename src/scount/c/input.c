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
#include <stdio.h>
#include <string.h>

#include "reckon/reckon.h"
#include "scount.h"

static const char* STDIN_FNAME = "stdin";

RcnCountStatistics* readStdinToCountStatistics(const char* extension) {
    const char* ext = extension;
    if (ext && ext[0] == '.') {
        ext = ext + 1;
    }
    if (!ext || ext[0] == '\0') {
        ext = "txt";
    }

    RcnCountStatistics* stats = rcnAllocCountStatistics(1);
    if (!stats) {
        return NULL; // LCOV_EXCL_LINE
    }

    // stdin + "." + ext + '\0'
    const size_t pathLength = strlen(STDIN_FNAME) + 1 + strlen(ext) + 1;
    char* filePath = malloc(pathLength);
    if (!filePath) {
        // LCOV_EXCL_START
        rcnFreeCountStatistics(stats);
        return NULL;
        // LCOV_EXCL_STOP
    }
    (void) snprintf(filePath, pathLength, "%s.%s", STDIN_FNAME, ext);
    RcnSourceFile* file = &stats->count.files[0];
    file->path = filePath;
    file->name = filePath;
    file->extension = filePath + strlen(STDIN_FNAME) + 1; // skip the dot

    size_t capacity = 4096;
    size_t size = 0;
    char* buffer = malloc(capacity + 1);
    if (!buffer) {
        // LCOV_EXCL_START
        rcnFreeCountStatistics(stats);
        return NULL;
        // LCOV_EXCL_STOP
    }

    char chunk[2048];
    for (;;) {
        const size_t readCount = fread(chunk, 1, sizeof(chunk), stdin);
        if (readCount > 0) {
            if (size + readCount >= capacity) {
                const size_t newCapacity = (size + readCount) * 2;
                char* newBuffer = realloc(buffer, newCapacity + 1);
                if (!newBuffer) {
                    // LCOV_EXCL_START
                    free(buffer);
                    rcnFreeCountStatistics(stats);
                    return NULL;
                    // LCOV_EXCL_STOP
                }
                buffer = newBuffer;
                capacity = newCapacity;
            }
            memcpy(buffer + size, chunk, readCount);
            size += readCount;
        }
        if (readCount < sizeof(chunk)) {
            if (ferror(stdin)) {
                // LCOV_EXCL_START
                free(buffer);
                rcnFreeCountStatistics(stats);
                return NULL;
                // LCOV_EXCL_STOP
            }
            break;
        }
    }
    buffer[size] = '\0';

    file->content.text = buffer;
    file->content.size = size;
    file->isContentRead = true;
    return stats;
}
