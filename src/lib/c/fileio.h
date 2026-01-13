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

/**
 * Interactions with the file system.
 *
 * Provides file I/O utilities for the Reckon library, including functions for
 * reading source files from the file system, finding source files in a
 * directory, and managing source file content in memory.
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "reckon/reckon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A list of source files.
 * 
 * Use `newSourceFileList()` to scan for files in a directory.
 * Ownership passes to the caller of `newSourceFileList()`.
 * The list must be deallocated with `freeSourceFileList()`.
 * If `size` is zero or `ok` is `false`, then `files` is `NULL`.
 */
typedef struct SourceFileList {
    RcnSourceFile* files;
    size_t size;
    size_t capacity;
    bool ok;
} SourceFileList;

/**
 * A stack of directory paths for iterative directory traversal.
 * 
 * Use `dirStackPush()` and `dirStackPop()` to interact.
 * The stack does not own its individual data entries. It is only used
 * to manage processing order.
 */
typedef struct DirStack {
    char** data;
    size_t size;
    size_t capacity;
} DirStack;

/**
 * The base path of a directory to be scanned for more files.
 * 
 * Is used as an intermediary data container in directory traversal operations.
 */
typedef struct BaseDir {
    const char* path;
    size_t length;
    bool hasTrailingSeparator;
} BaseDir;

/**
 * The result type of the `detectSourceFormat()` function.
 * 
 * Indicates whether the text format is supported and which format or
 * programming language was detected. If `isSupportedFormat` is `false`,
 * then the value of `format` is undefined and should not be used.
 * A `true` value of `isProgrammingLanguage` implies that `isSupportedFormat`
 * is also `true` and that the detected format represents a
 * supported programming language.
 */
typedef struct SourceFormatDetection {
    RcnTextFormat format;
    bool isSupportedFormat;
    bool isProgrammingLanguage;
} SourceFormatDetection;

/**
 * Appends a new source file with the given path to the list.
 * 
 * Returns `true` on success, `false` on failure.
 * On failure, the list remains unchanged.
 */
bool appendFile(SourceFileList* list, const char* path);

/**
 * Pushes a new directory path onto the stack.
 * 
 * Returns `true` on success, `false` on failure.
 * On failure, the stack remains unchanged.
 * The caller retains ownership of the `path` argument.
 */
bool dirStackPush(DirStack* stack, char* path);

/**
 * Pops a directory path from the stack.
 * 
 * Returns the popped path, or `NULL` if the stack is empty.
 * The caller takes ownership of the returned path.
 */
char* dirStackPop(DirStack* stack);

/**
 * Tests whether the given file system path refers to an existing directory.
 */
bool isDirectory(const char* path);

/**
 * Checks whether the given path is a valid input for
 * creating `RcnCountStatistics` structs.
 * Returns `NULL` if path is valid, or an error message describing the error.
 * The caller does not own any errors messages.
 */
const char* isValidStatsInput(const char* path);

/**
 * Allocates and initializes a single `RcnSourceFile`.
 *
 * Content is not loaded. Returns `NULL` on failure or if `path` is `NULL`.
 */
RcnSourceFile* newSourceFile(const char* path);

/**
 * Initializes a single `RcnSourceFile` with the given path.
 *
 * Content is not loaded. The file and path arguments must not be `NULL`
 * and a copy of `path` will be owned by the specified source file.
 */
void initSourceFile(RcnSourceFile* file, const char* path);

/**
 * Deinitializes a single `RcnSourceFile`, releasing any resources but
 * not freeing the pointer. The file argument may be `NULL`.
 */
void deinitSourceFile(RcnSourceFile* file);

/**
 * Frees a single `RcnSourceFile` and its owned members.
 * The file argument may be `NULL`.
 */
void freeSourceFile(RcnSourceFile* file);

/**
 * Creates a new list of all regular files under the given path.
 *
 * The specified path must denote an existing directory.
 * The returned list is sorted lexicographically by the file name
 * in ascending order. The returned list is owned by the caller
 * and must be deallocated with `freeSourceFileList()`.
 * `SourceFileList.ok` is `false` if an error occurred during scanning
 * in which case `files` is `NULL`. Otherwise, in case of a successful
 * scan, `SourceFileList.ok` is set to `true` to indicate success.
 * The file list might still be empty if the underlying directory is empty
 * or contains no regular files.
 */
SourceFileList newSourceFileList(const char* path);

/**
 * Frees the allocated memory for the given list of source files,
 * including all source file content.
 */
void freeSourceFileList(SourceFileList* list);

/**
 * Performs lightweight text format detection for a file.
 *
 * Detection currently relies solely on the file extension.
 */
SourceFormatDetection detectSourceFormat(const RcnSourceFile* file);

/**
 * Loads the entire file content into memory.
 *
 * On success, `file->content.text` points to a null-terminated buffer,
 * `file->size` is set to the size of the file on disk
 * and `file->isContentRead` is set to `true`. Sets `file->status` to
 * indicate potential errors. `file->status` must be `RCN_FILE_OP_OK`
 * before calling this function.
 * Returns `true` on success, `false` on failure.
 */
bool readSourceFileContent(RcnSourceFile* file);

/**
 * Releases any previously loaded file content.
 *
 * Resets `size` and `isContentRead`. Safe to call multiple times and
 * does nothing if no content is loaded.
 */
void freeSourceFileContent(RcnSourceFile* file);

#ifdef __cplusplus
}
#endif
