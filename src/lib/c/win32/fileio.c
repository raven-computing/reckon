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

static wchar_t* utf8ToWide(const char* str) {
    const int wideLength = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (wideLength <= 0) {
        return NULL;
    }
    wchar_t* wideStr = malloc((size_t) wideLength * sizeof(wchar_t));
    if (!wideStr) {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wideStr, wideLength);
    return wideStr;
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

FILE* fopenImpl(const char* path, const char* mode) {
    const wchar_t* wPath = utf8ToWide(path);
    if (!wPath) {
        return NULL;
    }
    wchar_t* wMode = utf8ToWide(mode);
    if (!wMode) {
        free((void*) wPath);
        return NULL;
    }
    FILE* handle = _wfopen(wPath, wMode);
    free((void*) wPath);
    free(wMode);
    return handle;
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

    wchar_t* wSearchPattern = utf8ToWide(searchPattern);
    free(searchPattern);
    if (!wSearchPattern) {
        return;
    }
    WIN32_FIND_DATAW findData;
    HANDLE found = FindFirstFileW(wSearchPattern, &findData);
    free(wSearchPattern);
    if (found == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        const wchar_t* wName = findData.cFileName;
        if (!wName || wName[0] == L'.') {
            continue;
        }
        const int nameUtf8Len = WideCharToMultiByte(
            CP_UTF8, 0, wName, -1, NULL, 0, NULL, NULL
        );
        if (nameUtf8Len <= 0) {
            continue;
        }
        char* name = malloc((size_t) nameUtf8Len);
        if (!name) {
            continue;
        }
        WideCharToMultiByte(
            CP_UTF8, 0, wName, -1, name, nameUtf8Len, NULL, NULL
        );
        const size_t nameLength = (size_t) nameUtf8Len - 1;
        const size_t fullLength = (
            pathLength
            + (trailingSep ? 0 : 1)
            + nameLength
            + 1
        );
        char* fullPath = malloc(fullLength);
        if (!fullPath) {
            free(name);
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
        } else if (isDirectory) {
            if (shouldIgnoreDirectory(name)) {
                free(name);
                free(fullPath);
                continue;
            }
            dirStackPush(stack, fullPath);
            free(name);
            continue;
        }
        free(name);
        free(fullPath);
    } while (FindNextFileW(found, &findData));

    FindClose(found);
}

bool isDirectory(const char* path) {
    if (!path) {
        return false;
    }
    wchar_t* wPath = utf8ToWide(path);
    if (!wPath) {
        return false;
    }
    DWORD attributes = GetFileAttributesW(wPath);
    free(wPath);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

const char* isValidStatsInput(const char* path) {
    assert(path != NULL);
    wchar_t* wPath = utf8ToWide(path);
    if (!wPath) {
        return "Out of memory";
    }
    DWORD attributes = GetFileAttributesW(wPath);
    free(wPath);
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
