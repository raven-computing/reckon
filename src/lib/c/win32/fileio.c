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

#ifdef _WIN32

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <windows.h>

#include "reckon/reckon.h"
#include "fileio.h"

static char getPathSeparatorToUse(const char* path, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (path[i] == '\\') {
            return '\\';
        }
    }
    return '/';
}

static inline bool isRegularFileAttr(DWORD attributes) {
    return (
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0
        && (attributes & FILE_ATTRIBUTE_DEVICE) == 0
        && (attributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0
    );
}

char* findFilenameImpl(const char* path) {
    char* backSlash = strrchr(path, '\\');
    char* forwardSlash = strrchr(path, '/');
    if (!backSlash && !forwardSlash) {
        return (char*) path;
    }
    if (!backSlash) {
        return forwardSlash;
    }
    if (!forwardSlash) {
        return backSlash;
    }
    return backSlash > forwardSlash ? backSlash : forwardSlash;
}

bool hasTrailingSeparatorImpl(const char* path, size_t length) {
    return (
        length > 0
        && (path[length - 1] == '/' || path[length - 1] == '\\')
    );
}

void scanDirectory(char* dirPath, DirStack* stack, SourceFileList* list) {
    const size_t pathLength = strlen(dirPath);
    const bool trailingSep = hasTrailingSeparatorImpl(dirPath, pathLength);
    // Search pattern: dirPath + ("*" or "\*")
    const size_t patternLength = pathLength + (trailingSep ? 1 : 2) + 1;
    char* searchPattern = malloc(patternLength);
    if (!searchPattern) {
        return;
    }
    if (trailingSep) {
        snprintf(searchPattern, patternLength, "%s*", dirPath);
    } else {
        snprintf(searchPattern, patternLength, "%s\\*", dirPath);
    }

    WIN32_FIND_DATAA findData;
    HANDLE found = FindFirstFileA(searchPattern, &findData);
    free(searchPattern);
    if (found == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        const char* name = findData.cFileName;
        if (!name || name[0] == '.') {
            continue;
        }
        const size_t nameLength = strlen(name);
        const size_t fullLength = (
            pathLength
            + (trailingSep ? 0 : 1)
            + nameLength
            + 1
        );
        char* fullPath = malloc(fullLength);
        if (!fullPath) {
            continue;
        }
        if (trailingSep) {
            snprintf(fullPath, fullLength, "%s%s", dirPath, name);
        } else {
            const char separator = getPathSeparatorToUse(dirPath, pathLength);
            snprintf(
                fullPath,
                fullLength,
                "%s%c%s",
                dirPath,
                separator,
                name
            );
        }

        DWORD attributes = findData.dwFileAttributes;
        const bool isDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        const bool isRegularFile = isRegularFileAttr(attributes);
        if (isRegularFile) {
            appendFile(list, fullPath);
        }
        if (isDirectory) {
            dirStackPush(stack, fullPath);
            continue;
        }
        free(fullPath);
    } while (FindNextFileA(found, &findData));

    FindClose(found);
}

bool isDirectory(const char* path) {
    if (!path) {
        return false;
    }
    DWORD attributes = GetFileAttributesA(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

const char* isValidStatsInput(const char* path) {
    assert(path != NULL);
    DWORD attributes = GetFileAttributesA(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        switch (error) {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
                return "No such file or directory";
            case ERROR_ACCESS_DENIED:
                return "Permission denied";
            case ERROR_FILENAME_EXCED_RANGE:
                return "Path name is too long";
            case ERROR_NOT_ENOUGH_MEMORY:
                return "Out of memory";
            case ERROR_IO_DEVICE:
                return "I/O error";
            default:
                return "Invalid input file path";
        }
    }
    const bool isDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    const bool isRegularFile = isRegularFileAttr(attributes);
    if (isDirectory || isRegularFile) {
        return NULL;
    }
    return "Is not a regular file or directory";
}

#endif // _WIN32
