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
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "reckon/reckon.h"
#include "fileio.h"

/* Declarations of platform-specific implementation functions */

char* findFilenameImpl(const char* path);
bool hasTrailingSeparatorImpl(const char* path, size_t length);

/**
 * Scans the given directory for regular files and appends them to the list.
 * Subdirectories are pushed onto the stack for further scanning. Entries that
 * have a different file type are ignored.
 */
void scanDirectory(char* dirPath, DirStack* stack, SourceFileList* list);

/* End of declarations */

/**
 * The initial capacity for allocated data structures.
 */
static const size_t DATA_STRUCT_CAP_INIT = 16;

/**
 * The factor by which the size of data structures grows when space is needed.
 */
static const size_t DATA_STRUCT_CAP_GROW_FACTOR = 2;

/**
 * The maximum size of a source file that will be processed.
 * Files larger than this size will be skipped during reading.
 */
static const size_t FILE_MAX_PROC_SIZE = 512UL * 1024UL * 1024UL;

/**
 * The maximum number of `RcnSourceFile` objects that can be
 * tracked in a `SourceFileList`. This is an arbitrary limit imposed to prevent
 * resource exhaustion and infinite loops during directory traversals
 * due to e.g. symlink loops. Under normal circumstances, this limit is high
 * enough and should not be reached.
 */
static const size_t FILES_LIST_MAX_SIZE = 10000;

static char* findFilename(const char* path) {
    if (!path) {
        return NULL;
    }
    char* separator = findFilenameImpl(path);
    return (path == separator) ? separator : separator + 1;
}

static char* findExtension(const char* name) {
    if (!name) {
        return NULL;
    }
    char* dot = strrchr(name, '.');
    if (!dot || *(dot + 1) == '\0') {
        return NULL;
    }
    return dot + 1;
}

/**
 * Comparator for qsort(), lexicographical sort by file name.
 */
static int compareSourceFileByName(const void* arg1, const void* arg2) {
    const RcnSourceFile* file1 = (const RcnSourceFile*) arg1;
    const RcnSourceFile* file2 = (const RcnSourceFile*) arg2;
    if (!file1->name) {
        return file2->name ? -1 : 0; // LCOV_EXCL_LINE
    }
    if (!file2->name) {
        return 1; // LCOV_EXCL_LINE
    }
    return strcmp(file1->name, file2->name);
}

static void trimExactSize(SourceFileList* list) {
    if (list->size > 0 && list->capacity > list->size) {
        RcnSourceFile* reallocatedFiles = realloc(
            list->files,
            list->size * sizeof(RcnSourceFile)
        );
        if (reallocatedFiles) {
            list->files = reallocatedFiles;
            list->capacity = list->size;
        }
    }
}

static bool finishFileRd(
    FILE* handle,
    RcnSourceFile* file,
    RcnFileOpStatus status
) {
    // Do not mask an earlier error
    if (fclose(handle) != 0 && status == RCN_FILE_OP_OK) {
        status = RCN_FILE_OP_IO_ERROR; // LCOV_EXCL_LINE
    }
    file->status = status;
    return status == RCN_FILE_OP_OK ? true : false;
}

bool appendFile(SourceFileList* list, const char* path) {
    if (list->size >= FILES_LIST_MAX_SIZE) {
        return false; // LCOV_EXCL_LINE
    }
    if (list->size >= list->capacity) {
        const size_t newCapacity = (
            list->capacity
            ? list->capacity * DATA_STRUCT_CAP_GROW_FACTOR
            : DATA_STRUCT_CAP_INIT
        );
        RcnSourceFile* reallocatedFiles = realloc(
            list->files,
            newCapacity * sizeof(RcnSourceFile)
        );
        if (!reallocatedFiles) {
            return false;
        }
        list->files = reallocatedFiles;
        list->capacity = newCapacity;
    }
    initSourceFile(&list->files[list->size++], path);
    return true;
}

bool dirStackPush(DirStack* stack, char* path) {
    if (!path) {
        return false;
    }
    if (stack->size == stack->capacity) {
        const size_t newCapacity = (
            stack->capacity
            ? stack->capacity * DATA_STRUCT_CAP_GROW_FACTOR
            : DATA_STRUCT_CAP_INIT
        );
        char** reallocatedStackData = (char**) realloc(
            (void*) stack->data,
            newCapacity * sizeof(char*)
        );
        if (!reallocatedStackData) {
            return false;
        }
        stack->data = reallocatedStackData;
        stack->capacity = newCapacity;
    }
    stack->data[stack->size++] = path;
    return true;
}

char* dirStackPop(DirStack* stack) {
    if (stack->size == 0) {
        return NULL;
    }
    stack->size--;
    return stack->data[stack->size];
}

RcnSourceFile* newSourceFile(const char* path) {
    if (!path) {
        return NULL;
    }
    RcnSourceFile* file = malloc(sizeof(RcnSourceFile));
    if (!file) {
        return NULL;
    }
    initSourceFile(file, path);
    return file;
}

void initSourceFile(RcnSourceFile* file, const char* path) {
    assert(file != NULL);
    assert(path != NULL);
    RcnFileOpStatus status = RCN_FILE_OP_OK;
    char* pathCopy = strdup(path);
    if (!pathCopy) {
        // LCOV_EXCL_START
        status = (
            errno == ENOMEM
            ? RCN_FILE_OP_ALLOC_FAILURE
            : RCN_FILE_OP_UNKNOWN_ERROR
        );
        // LCOV_EXCL_STOP
    }
    file->path = pathCopy;
    file->name = findFilename(file->path);
    file->extension = findExtension(file->name);
    file->content = (RcnSourceText){0};
    file->status = status;
    file->isContentRead = false;
}

void deinitSourceFile(RcnSourceFile* file) {
    if (file) {
        if (file->path) {
            free(file->path);
            file->path = NULL;
        }
        if (file->name) {
            file->name = NULL;
        }
        if (file->extension) {
            file->extension = NULL;
        }
        freeSourceFileContent(file);
    }
}

void freeSourceFile(RcnSourceFile* file) {
    deinitSourceFile(file);
    free(file);
}

// Ignore false positive. Perhaps it gets confused by a combination of realloc
// usage in trimExactSize() and the optimization that a file name and extension
// inside a RcnSourceFile are pointers into the copy of the path during init
// and not independent copies.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
#endif
SourceFileList newSourceFileList(const char* path) {
    SourceFileList list = {0};
    if (!path) {
        return list;
    }
    DirStack stack = {0};
    char* dirPath = strdup(path);
    if (!dirStackPush(&stack, dirPath)) {
        free(dirPath);
        return list;
    }

    while ((dirPath = dirStackPop(&stack)) != NULL) {
        scanDirectory(dirPath, &stack, &list);
        free(dirPath);
        if (list.size >= FILES_LIST_MAX_SIZE) {
            break; // LCOV_EXCL_LINE
        }
    }

    free((void*) stack.data);
    trimExactSize(&list);
    if (list.size > 1) {
        qsort(
            list.files,
            list.size,
            sizeof(RcnSourceFile),
            compareSourceFileByName
        );
    }
    list.ok = true;
    return list;
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

void freeSourceFileList(SourceFileList* list) {
    if (list) {
        if (list->files) {
            for (size_t i = 0; i < list->size; ++i) {
                deinitSourceFile(&list->files[i]);
            }
            free(list->files);
            list->files = NULL;
        }
        list->size = 0;
        list->capacity = 0;
    }
}

bool readSourceFileContent(RcnSourceFile* file) {
    if (!file) {
        return false;
    }
    if (file->status != RCN_FILE_OP_OK) {
        return false;
    }
    if (file->isContentRead) {
        return true;
    }
    if (!file->path) {
        file->status = RCN_FILE_OP_INVALID_PATH;
        return false;
    }
    FILE* handle = fopen(file->path, "rb");
    if (!handle) {
        file->status = (
            errno == ENOENT
            ? RCN_FILE_OP_FILE_NOT_FOUND
            : RCN_FILE_OP_IO_ERROR
        );
        return false;
    }

    if (fseek(handle, 0, SEEK_END) != 0) {
        return finishFileRd(handle, file, RCN_FILE_OP_IO_ERROR);
    }
    const long endPos = ftell(handle);
    if (endPos < 0) {
        return finishFileRd(handle, file, RCN_FILE_OP_IO_ERROR);
    }
    const size_t length = (size_t) endPos;
    const size_t bufferSize = length + 1;
    if (fseek(handle, 0, SEEK_SET) != 0) {
        return finishFileRd(handle, file, RCN_FILE_OP_IO_ERROR);
    }
    if (length > FILE_MAX_PROC_SIZE) {
        return finishFileRd(handle, file, RCN_FILE_OP_FILE_TOO_LARGE);
    }
    char* content = malloc(bufferSize);
    if (!content) {
        return finishFileRd(handle, file, RCN_FILE_OP_ALLOC_FAILURE);
    }
    if (fread(content, 1, length, handle) != length) {
        free(content);
        return finishFileRd(handle, file, RCN_FILE_OP_IO_ERROR);
    }
    content[length] = '\0';

    file->content = (RcnSourceText){
        .text = content,
        .size = length
    };
    file->isContentRead = true;
    return finishFileRd(handle, file, RCN_FILE_OP_OK);
}

void freeSourceFileContent(RcnSourceFile* file) {
    if (file) {
        if (file->content.text) {
            free(file->content.text);
        }
        file->content = (RcnSourceText){0};
        file->isContentRead = false;
    }
}
