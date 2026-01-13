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

#ifdef __linux__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <sys/stat.h>

#include "reckon/reckon.h"
#include "fileio.h"

/**
 * Constructs the full file path for a given directory entry,
 * i.e. a child in the `base` directory.
 */
static char* fullFilePath(BaseDir* base , struct dirent* file) {
    const size_t nameLength = strlen(file->d_name);
    const size_t fullLength = (
        base->length
        + (base->hasTrailingSeparator ? 0 : 1)
        + nameLength
        + 1 // null-terminator
    );
    char* fullPath = malloc(fullLength);
    if (fullPath) {
        int status = snprintf(
            fullPath,
            fullLength,
            base->hasTrailingSeparator ? "%s%s" : "%s/%s",
            base->path,
            file->d_name
        );
        if (status < 0) {
            free(fullPath); // LCOV_EXCL_LINE
            return NULL; // LCOV_EXCL_LINE
        }
    }
    return fullPath;
}

char* findFilenameImpl(const char* path) {
    char* slash = strrchr(path, '/');
    return slash ? slash : (char*) path;
}

bool hasTrailingSeparatorImpl(const char* path, size_t length) {
    return (length > 0 && path[length - 1] == '/');
}

void scanDirectory(char* dirPath, DirStack* stack, SourceFileList* list) {
    DIR* directory = opendir(dirPath);
    if (!directory) {
        return;
    }
    const size_t pathLength = strlen(dirPath);
    BaseDir base = {
        .path = dirPath,
        .length = pathLength,
        .hasTrailingSeparator = hasTrailingSeparatorImpl(dirPath, pathLength)
    };
    struct dirent* entry = NULL;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue; // Skip '.', '..' and hidden files, etc.
        }
        char* fullPath = fullFilePath(&base, entry);
        if (!fullPath) {
            continue;
        }
        bool entryIsDirectory = false;
        bool entryIsRegularFile = false;
        struct stat attr;
        if (lstat(fullPath, &attr) == 0) {
            if (!S_ISLNK(attr.st_mode)) {
                entryIsDirectory = S_ISDIR(attr.st_mode);
                entryIsRegularFile = S_ISREG(attr.st_mode);
            }
        }
        if (entryIsRegularFile) {
            appendFile(list, fullPath);
        }
        if (entryIsDirectory) {
            dirStackPush(stack, fullPath);
            continue;
        }
        free(fullPath);
    }
    closedir(directory);
}

bool isDirectory(const char* path) {
    assert(path != NULL);
    struct stat attr;
    if (lstat(path, &attr) != 0) {
        return false;
    }
    return S_ISDIR(attr.st_mode);
}

const char* isValidStatsInput(const char* path) {
    assert(path != NULL);
    struct stat attr;
    if (lstat(path, &attr) != 0) {
        // Not using strerror(errno) due to lack of predictability,
        // ownership, and message localization
        switch (errno) {
            case ENOENT:
                return "No such file or directory";
            // LCOV_EXCL_START
            case EACCES:
                return "Permission denied";
            case ENAMETOOLONG:
                return "Path name is too long";
            case EIO:
                return "I/O error";
            case ENOMEM:
                return "Out of memory";
            default:
                return "Invalid input file path";
            // LCOV_EXCL_STOP
        }
    }
    const bool isDirectory = S_ISDIR(attr.st_mode);
    const bool isRegularFile = S_ISREG(attr.st_mode);
    if (isDirectory || isRegularFile) {
        return NULL;
    }
    return "Is not a regular file or directory"; // LCOV_EXCL_LINE
}

#endif // __linux__
