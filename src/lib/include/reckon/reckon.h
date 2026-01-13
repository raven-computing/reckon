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
 * @file reckon.h
 * @brief The primary API of the Reckon library.
 *
 * Exposes types and function declarations for source code metrics.
 * Provides functionality to count the occurrences of various source code
 * related concepts such as number of words, physical lines and logical lines
 * of code, and other related metrics. The library supports multiple
 * programming languages and file formats. Supported formats are
 * enumerated by the `RcnTextFormat` enum.
 * 
 * The Reckon library only supports processing text that is encoded in UTF-8
 * or UTF-16. In the case of UTF-16, a BOM must be present at the start of
 * the text to indicate endianness. For any operation provided by the library,
 * if the input text has encoding errors, the operation finishes gracefully but
 * the computed result is undefined.
 * 
 * The typical usage is to create a `RcnCountStatistics` struct for either
 * a single file or directory path using the `rcnCreateCountStatistics()` 
 * function. Choose the desired counting operations, formats and other options
 * using a `RcnStatOptions` struct. Then pass both the created statistics and
 * options to the `rcnCount()` function to perform the counting. Finally, after
 * having evaluated the computed statistics, free the allocated statistics
 * using the `rcnFreeCountStatistics()` function.
 * 
 * What follows are definitions of metrics that are computed by this library.
 * 
 * * Logical Lines of Code (LLC):  
 * The number of programming-language-specific, non-empty, non-comment program
 * source constructs that correspond each to one complete semantically cohesive
 * statement or declaration in the grammatical sense of the underlying
 * language, counted independently of physical line breaks, formatting and
 * other visual layout aspects. Logical lines in a source code file are
 * partitions within the top-level statement/declaration units recognized by
 * the language grammar or an approximation thereof. Such units include, but
 * are not limited to, executable statements (e.g. expression statements,
 * return, if, for, while, switch cases), declarations/definitions
 * (e.g. variable, function, type/class definitions), other language-defined
 * standalone constructs (e.g. import/use/module directives). The LLC count is
 * the number of such units after segmentation. Thus, and in comparison to
 * physical lines of code, multiple statements on one physical line count as
 * multiple LLCs. One statement spanning multiple physical lines counts
 * as one LLC.
 * 
 * * Physical Lines (PHL):  
 * The number of hard physical lines in the source text, including blank lines
 * and comments.
 * 
 * * Words (WRD):  
 * The number of non-zero-length sequences of printable characters delimited
 * by white space.
 * 
 * * Characters (CHR):  
 * The number of Unicode code points. This includes printable as well as
 * non-printable characters. Therefore, this metric includes
 * control characters, like newlines.
 * 
 * Please note that the above definitions themselves are not strictly formal
 * and not part of the API contract. Both the definitions as well as the
 * library implementation may evolve in future releases, such that different
 * versions of the Reckon library may compute slightly different results for a
 * particular metric and input combination.
 * For more information, please refer to the official Reckon documentation.
 * 
 * The functions in this library are not MT-safe.
 *
 * @see https://docs.raven-computing.com/reckon/latest
 * @author Phil Gaiser
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "reckon_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The total number of supported text formats, including
 * supported programming languages.
 */
#define RECKON_NUM_SUPPORTED_FORMATS 3

/**
 * Macro to create a format option bitmask.
 * 
 * Users should prefer to use the `RcnFormatOption` enumeration,
 * instead of using this macro directly.
 * 
 * @param frmt The RcnTextFormat enumerator value.
 */
#define RECKON_MK_FRMT_OPT(frmt) (1ULL << (frmt))

/**
 * The name of the environment variable to control debug logging.
 * 
 * If the environment has a variable with this name set to "1", then debug
 * logging is enabled and for certain operations additional information is
 * printed on stdout. A variable value of "0" disables all debug logging.
 * If the environment variable is not set, debug logging is disabled
 * by default.
 * The definition of the environment variable only has an effect if the
 * library is compiled as a debug build.
 */
#define RECKON_ENV_VAR_DEBUG "RECKON_DEBUG"

/**
 * Enumeration of supported text formats and programming languages.
 * 
 * Users should not rely on the numeric enumerator value as it may change when
 * support for new formats or programming languages is added in the future and
 * the enumerators are reordered.
 */
typedef enum RcnTextFormat {

    /**
     * Text with no specific formatting, as usually found in
     * files with a '.txt' extension.
     */
    RCN_TEXT_UNFORMATTED = 0,

    /**
     * Source files for the C programming language.
     */
    RCN_LANG_C = 1,

    /**
     * Source files for the Java programming language.
     */
    RCN_LANG_JAVA = 2

} RcnTextFormat;

/**
 * Enumeration of error states.
 * 
 * All count operations return a `RcnResultState` struct that contains
 * information about the operation's success or failure, and in latter case,
 * the error code indicates the type of error that has occurred.
 */
typedef enum RcnErrorCode {

    /**
     * No error has occurred.
     */
    RCN_ERR_NONE,

    /**
     * The input format or programming language is not supported.
     */
    RCN_ERR_UNSUPPORTED_FORMAT,

    /**
     * The input provided was invalid.
     */
    RCN_ERR_INVALID_INPUT,

    /**
     * The input is too large to be processed.
     * 
     * This indicates that the input size exceeds internal limits.
     */
    RCN_ERR_INPUT_TOO_LARGE,

    /**
     * A syntax error was detected.
     * 
     * This usually indicates that an attempt was made to parse programming
     * language source text that is syntactically incorrect in that specific
     * programming language.
     */
    RCN_ERR_SYNTAX_ERROR,

    /**
     * A memory allocation failure has occurred.
     * 
     * This usually indicates that the system is out of memory (OOM error).
     */
    RCN_ERR_ALLOC_FAILURE,

    /**
     * An unknown error has occurred.
     * 
     * This is used as a catch-all for errors that are not further specified.
     */
    RCN_ERR_UNKNOWN

} RcnErrorCode;

/**
 * The result status type of an operation indicating success or failure.
 * 
 * Count operations return result types that contain this type of state.
 * For a single operation, e.g. `rcnCountLogicalLines()`, an `ok` value
 * of `true` indicates that the operation was fully successful, implying
 * that `errorCode` is set to `RCN_ERR_NONE` and `errorMessage` is `NULL`.
 * Therefore, if `ok` is `false`, then `errorCode` indicates the type of error
 * that has occurred and `errorMessage` may or may not be set to provide
 * additional information.
 * For compound operations, e.g. `rcnCount()`, an `ok` value of `true`
 * indicates that the operation was at least partially successful and parts of
 * the computed compound result are usable. In such a case, `errorCode` may
 * still indicate one of the encountered errors, usually the last encountered
 * one, and `errorMessage` may or may not provide more information. This
 * implies that for compound operations an `ok` value of `true` might only
 * indicate that no critical error has occurred.
 */
typedef struct RcnResultState {

    /**
     * The error code indicating the type of error that has occurred.
     * 
     * Is either `RCN_ERR_NONE` if no error was detected,
     * or set to an appropriate error code.
     */
    RcnErrorCode errorCode;

    /**
     * An optional error message describing the encountered error.
     * 
     * Messages are not strictly guaranteed to be presentable to end users,
     * as they may contain technical details. May be `NULL`.
     * A user does not take ownership of the string and must
     * not attempt to free it.
     */
    const char* errorMessage;

    /**
     * Whether the operation has completed without critical errors.
     * 
     * Is initialized as `false` and only set to `true` if the last
     * performed operation was fully or partially successful. Therefore, it
     * remains `false` if the underlying entity was not processed for any
     * reason. Indicates full success for single operations and potentially
     * partial success without critical errors for compound operations.
     */
    bool ok;

} RcnResultState;

/**
 * A count number of some metric within source text.
 * 
 * This type is used to represent, for example, the count of lines within
 * source text. Shall be treated as a non-negative integer number
 * of arbitrary bit width. In the unlikely event of an overflow, count values
 * wrap around according to standard unsigned integer arithmetic.
 */
typedef uint64_t RcnCount;

/**
 * The result type for a single code analysis operation.
 * 
 * Represents the end result of one concrete type of count operation.
 * For example, it will only contain the count of logical lines of code, or
 * only the count of physical lines, depending on the operation performed.
 */
typedef struct RcnCountResult {

    /**
     * The counted result number.
     */
    RcnCount count;

    /**
     * The result state of the operation, indicating success or failure.
     */
    RcnResultState state;

} RcnCountResult;

/**
 * Result type for a group of analysis operations on a single source entity.
 * 
 * Represents the end results of possibly multiple count operations performed
 * on a single named source entity, like a specific source file. This is used
 * to group multiple different count metrics together into a single type.
 */
typedef struct RcnCountResultGroup {

    /**
     * The counted logical lines of code.
     */
    RcnCount logicalLines;

    /**
     * The counted hard physical lines.
     */
    RcnCount physicalLines;

    /**
     * The counted words.
     */
    RcnCount words;

    /**
     * The counted characters.
     */
    RcnCount characters;

    /**
     * The size of the source entity in bytes.
     */
    RcnCount sourceSize;

    /**
     * The state of performed operations, indicating success or failure.
     */
    RcnResultState state;

    /**
     * Indicates whether the source entity was actually processed.
     * 
     * If this is `false`, then no counts are available and all count values
     * are zero. This may happen if the source entity's format is not
     * supported, was not selected in a performed analysis operation, an
     * attempt to read the source file content failed, or some other error
     * has occurred. A processed entity might still have zero counts, e.g. if
     * it is an empty file, and it might still have errors as indicated
     * in the `state` field.
     */
    bool isProcessed;

} RcnCountResultGroup;

/**
 * A block of source text.
 * 
 * Holds a pointer to the text content and its size in bytes.
 * The source text may or may not be null-terminated. A different type that is
 * composed of this type may further define this explicitly. The `text` field
 * may contain the bytes of text encoded in any of the supported encodings.
 */
typedef struct RcnSourceText {

    /**
     * The source text content, as a pointer to a byte array.
     */
    char* text;

    /**
     * The size of the source text in bytes.
     * 
     * If the `text` is null-terminated, the size does not include the
     * terminating null byte.
     */
    size_t size;

} RcnSourceText;

/**
 * Enumeration of file processing operation status codes.
 * 
 * `RcnSourceFile` structs carry this status to indicate the processing state
 * of the file, allowing to differentiate between various error conditions.
 * It is guaranteed that the status code indicating success (i.e. no error)
 * evaluates to zero, whereas all codes indicating a detected error evaluate
 * to non-zero values.
 */
typedef enum RcnFileOpStatus {

    /**
     * No error has occurred.
     */
    RCN_FILE_OP_OK,

    /**
     * A provided file path is invalid or malformed.
     * 
     * This could mean that a path was deemed invalid either by the
     * Reckon library or the operating system.
     */
    RCN_FILE_OP_INVALID_PATH,

    /**
     * The provided file was not found in the file system.
     */
    RCN_FILE_OP_FILE_NOT_FOUND,

    /**
     * An I/O error has occurred during file processing.
     * 
     * This could indicate issues such as permission denied, file not found,
     * or read/write errors.
     */
    RCN_FILE_OP_IO_ERROR,

    /**
     * A memory allocation failure has occurred during file processing.
     */
    RCN_FILE_OP_ALLOC_FAILURE,

    /**
     * The file is too large to be processed.
     * 
     * This indicates that the file size exceeds internal limits set by
     * the Reckon library.
     */
    RCN_FILE_OP_FILE_TOO_LARGE,

    /**
     * An unknown error has occurred.
     * 
     * This is used as a catch-all for errors that are not further specified.
     */
    RCN_FILE_OP_UNKNOWN_ERROR

} RcnFileOpStatus;

/**
 * A structure representing a text source file.
 * 
 * Holds metadata and content of a source file to be analyzed. A source file
 * may or may not contain source code written in a programming language. It may
 * as well be regular text, formatted or unformatted. Users typically don't
 * need to create, manipulate or manage the memory of this struct directly, as
 * it is managed internally by the Reckon library to represent and track files.
 * 
 * The file content may or may not be loaded at any given time. Check the
 * `isContentRead` field to determine if the content was read from the file
 * system. The `content.size` of a not yet read file is defined to be zero.
 * Thus, empty files that were read will have `isContentRead` equal to `true`,
 * `content.size` of zero and an empty string in `content.text`.
 */
typedef struct RcnSourceFile {

    /**
     * The absolute file path of the source code file.
     */
    char* path;

    /**
     * The name of the source code file, including the file extension.
     */
    char* name;

    /**
     * The file extension of the source code file, without a leading dot.
     */
    char* extension;

    /**
     * The text content of the source file.
     * 
     * The content is a null-terminated string. It may only be set
     * if `isContentRead` is `true`. The size is the actual size in bytes
     * of the file content on disk.
     */
    RcnSourceText content;

    /**
     * Indicates whether the content of the file has been read
     * from the file system.
     */
    bool isContentRead;

    /**
     * The status code indicating the processing state of the source code file.
     */
    RcnFileOpStatus status;

} RcnSourceFile;

/**
 * The count results for a set of source files.
 * 
 * Contains a list of source files that are subject to analysis, along
 * with their corresponding count results. Each file in the `files` list has a
 * corresponding result in the `results` list at the same index.
 * No checks are performed regarding duplicate files in the list, as a result,
 * uniqueness is not guaranteed.
 */
typedef struct RcnCountResultSet {

    /**
     * The list of files subject to analysis.
     * 
     * May be `NULL` if no files are set.
     */
    RcnSourceFile* files;

    /**
     * The list of results corresponding to the analyzed files.
     * 
     * May be `NULL` if no files are set.
     */
    RcnCountResultGroup* results;

    /**
     * The number of files in the `files` list.
     */
    size_t size;

    /**
     * The number of files in `files` that were analyzed.
     * 
     * This corresponds to the number of entries in `results` that
     * have `isProcessed` set to `true`.
     */
    size_t sizeProcessed;

} RcnCountResultSet;

/**
 * A collection of source code metrics.
 * 
 * This type is used to track and store the results for code analysis
 * operations. It contains statistics about a set of source code files, where
 * conceptually every text file that would be part of a source tree is
 * considered a source code file, even if it doesn't contain actual code.
 */
typedef struct RcnCountStatistics {

    /**
     * The total number of logical lines of code, across all files
     * and programming languages.
     */
    RcnCount totalLogicalLines;

    /**
     * The total number of hard physical lines, across all files and formats.
     */
    RcnCount totalPhysicalLines;

    /**
     * The total number of words, across all files and formats.
     */
    RcnCount totalWords;

    /**
     * The total number of characters, across all files and formats.
     */
    RcnCount totalCharacters;

    /**
     * The total size of the source code files, across all files and formats.
     * 
     * Measured in bytes.
     */
    RcnCount totalSourceSize;

    /**
     * The number of logical lines of code per supported programming language.
     * 
     * The index corresponds to the `RcnTextFormat` enumerator values.
     */
    RcnCount logicalLines[RECKON_NUM_SUPPORTED_FORMATS];

    /**
     * The number of hard physical lines per supported programming language.
     * 
     * The index corresponds to the `RcnTextFormat` enumerator values.
     */
    RcnCount physicalLines[RECKON_NUM_SUPPORTED_FORMATS];

    /**
     * The number of words per supported format.
     * 
     * The index corresponds to the `RcnTextFormat` enumerator values.
     */
    RcnCount words[RECKON_NUM_SUPPORTED_FORMATS];

    /**
     * The number of characters per supported format.
     * 
     * The index corresponds to the `RcnTextFormat` enumerator values.
     */
    RcnCount characters[RECKON_NUM_SUPPORTED_FORMATS];

    /**
     * The total size of the source code files per supported format.
     * 
     * The index corresponds to the `RcnTextFormat` enumerator values.
     * The size is measured in bytes.
     */
    RcnCount sourceSize[RECKON_NUM_SUPPORTED_FORMATS];

    /**
     * The set of results for each analyzed source code file.
     */
    RcnCountResultSet count;

    /**
     * The state of the compound operation, indicating success or failure.
     * 
     * In case the result set only contains one source file, this state
     * reflects the result state of the processing of that one source file and
     * is therefore equivalent to the state of the corresponding
     * only result group in the set.
     */
    RcnResultState state;

} RcnCountStatistics;

/**
 * Options to specify which counting operations to perform.
 * 
 * Users can combine multiple options using a bitwise OR operation.
 * Do not rely on concrete numeric enumerator values.
 */
typedef enum RcnCountOption {

    /**
     * Count the number of characters (CHR).
     * 
     * This metric includes control characters, like newlines. The count
     * therefore includes non-printable characters.
     */
    RCN_OPT_COUNT_CHARACTERS = 0x01,

    /**
     * Count the number of words (WRD).
     */
    RCN_OPT_COUNT_WORDS = 0x02,

    /**
     * Count hard physical lines (PHL).
     * 
     * This option includes all lines, including blank lines and comments.
     */
    RCN_OPT_COUNT_PHYSICAL_LINES = 0x04,

    /**
     * Count logical lines of code (LLC).
     * 
     * This option is generally only applicable to source files containing text
     * with a format that supports the notion of logical lines of code. This
     * includes files containing source code written in a programming language
     * but not, for example, plain text files (.txt).
     */
    RCN_OPT_COUNT_LOGICAL_LINES = 0x08

} RcnCountOption;

/**
 * Options for format-specific analysis behaviours.
 * 
 * Users can use these options to enable or disable specific formats when
 * processing source files. Multiple options can be combined using a
 * bitwise OR operation. Do not rely on concrete numeric enumerator values.
 */
typedef enum RcnFormatOption {

    /**
     * Option to select statistics for plain text files written without any
     * explicit formatting. These are usually files with a '.txt' extension.
     */
    RCN_OPT_TEXT_UNFORMATTED = RECKON_MK_FRMT_OPT(RCN_TEXT_UNFORMATTED),

    /**
     * Option to select statistics for source code files written in
     * the C programming language.
     */
    RCN_OPT_LANG_C = RECKON_MK_FRMT_OPT(RCN_LANG_C),

    /**
     * Option to select statistics for source code files written in
     * the Java programming language.
     */
    RCN_OPT_LANG_JAVA = RECKON_MK_FRMT_OPT(RCN_LANG_JAVA)

} RcnFormatOption;

/**
 * Options to customize the behaviour of counting operations.
 * 
 * Allows users to specify various options that control how counting
 * operations are performed.
 * 
 * A zero-initialized `RcnStatOptions` struct will select default behaviour.
 */
typedef struct RcnStatOptions {

    /**
     * Options to specify which counting operations to perform.
     * 
     * Use `RcnCountOption` options to enable or disable specific operations.
     * Operations can be combined using bitwise OR.
     * 
     * A value of zero (default) selects all available counting operations.
     */
    uint32_t operations;

    /**
     * Options to specify which text formats to include in the counting.
     * 
     * This is also used to specify programming languages to include.
     * Use `RcnFormatOption` options to enable or disable specific formats
     * and/or programming languages. Formats can be combined using bitwise OR.
     * 
     * A value of zero (default) selects all available text formats.
     */
    uint32_t formats;

    /**
     * Whether to stop the processing on the first encountered error.
     * 
     * If this is set to `true`, then compound functions like `rcnCount()` will
     * abort as soon as any error is encountered. Otherwise, a function
     * may or may not attempt to continue its operation when encountering
     * non-critical errors. Some errors, e.g. memory allocation failures, are
     * considered critical and may always abort an operation immediately
     * regardless of this option's value.
     */
    bool stopOnError;

    /**
     * Whether to keep the original file content in memory.
     * 
     * If this is set to `true`, then after processing a source file, the file
     * content will be retained in memory for potential future use. If this is
     * set to `false`, the original content may be discarded automatically
     * after processing in order to free up memory as soon as possible.
     */
    bool keepFileContent;

} RcnStatOptions;

/**
 * Creates a new `RcnCountStatistics` struct for the specified file path.
 *
 * The specified file path can denote either a single regular file or a
 * directory containing multiple files and subdirectories. In the case of a
 * directory, all regular files within the directory and subdirectories
 * therein will be part of the `RcnCountResultSet` of the returned statistics.
 * A relative file path will be interpreted as relative to the underlying
 * current working directory.
 *
 * A user takes ownership of the returned struct and must free it with
 * `rcnFreeCountStatistics()`.
 *
 * @param path A path in the file system. Is interpreted as a byte sequence in
 *             the underlying platform's native encoding.
 * @return A newly allocated `RcnCountStatistics` struct, or `NULL` on error.
 */
RECKON_EXPORT RcnCountStatistics* rcnCreateCountStatistics(const char* path);

/**
 * Frees a previously allocated `RcnCountStatistics` struct.
 * 
 * Must have been previously allocated using `rcnCreateCountStatistics()`.
 *
 * @param stats The `RcnCountStatistics` struct to free. May be `NULL`.
 */
RECKON_EXPORT void rcnFreeCountStatistics(RcnCountStatistics* stats);

/**
 * Performs counting operations using the specified statistics options.
 *
 * Processes the source files of the specified statistics and performs analysis
 * operations, e.g. counting the number of logical lines of code, according to
 * the given options. The files inside the given statistics must exist and be
 * readable regular text files.
 * 
 * This function is not idempotent with respect to the same stats struct.
 * Calling it multiple times on the same `RcnCountStatistics` struct is
 * undefined behaviour.
 *
 * @param stats The statistics to evaluate.
 * @param options Options to customize the analysis behaviour.
 */
RECKON_EXPORT void rcnCount(RcnCountStatistics* stats, RcnStatOptions options);

/**
 * Counts the number of logical lines of code in the specified source text.
 * 
 * See header documentation for details on how logical lines of code are
 * defined and for supported encodings.
 *
 * @param language The format of the specified source text. Must denote a
 *                 supported programming language.
 * @param sourceCode The source code text to count logical lines in.
 * @return A `RcnCountResult` struct containing the line count.
 */
RECKON_EXPORT RcnCountResult rcnCountLogicalLines(
    RcnTextFormat language,
    RcnSourceText sourceCode
);

/**
 * Marks the counted logical lines in the source code of the specified file.
 *
 * Reads the file located at the specified file system path and adds source
 * code comments to lines that are counted as logical lines of code.
 * The comments are according to the syntax of the underlying used programming
 * language and indicate the count number plus the type of syntactic construct
 * that contributes to the logical line count. One physical line of code can
 * contain an annotation for multiple logical lines. This function can only be
 * used for files that contain text formatted in a supported programming
 * language.
 * 
 * See header documentation for details on how logical lines of code are
 * defined. The text in the file must be encoded with UTF-8. Other encodings
 * are not supported by this function and result in undefined behaviour.
 *
 * @param path The file system path of the source code file to annotate.
 *             Is interpreted as a byte sequence in the underlying platform's
 *             native encoding. Relative paths are interpreted relative to the
 *             current working directory.
 * @return The read source code of the specified file with comments added to
 *         the counted lines, as a `RcnSourceText` with a null-terminated
 *         string. The caller takes ownership of the returned struct and must
 *         free it with `rcnFreeSourceText()`. Returns a struct with `text` set
 *         to `NULL` on error.
 */
RECKON_EXPORT RcnSourceText rcnMarkLogicalLinesInFile(const char* path);

/**
 * Marks the counted logical lines in the specified source code text.
 *
 * Creates a copy of the specified source code text and adds source code
 * comments to lines that are counted as logical lines of code. The comments
 * are according to the syntax of the used programming language and indicate
 * the count number plus the type of syntactic construct that contributes
 * to the logical line count. One physical line of code can contain an
 * annotation for multiple logical lines. This function can only be used with
 * `RcnTextFormat` enumerators that represent a supported programming language.
 * 
 * See header documentation for details on how logical lines of code are
 * defined. The specified source code text must be encoded with UTF-8. Other
 * encodings are not supported by this function and result
 * in undefined behaviour.
 *
 * @param language The format of the specified source code. Must denote a
 *                 supported programming language.
 * @param sourceCode The source code text to annotate.
 * @return A copy of the specified source code with comments added to the
 *         counted lines, as a `RcnSourceText` with a null-terminated
 *         string, regardless whether the input string is null-terminated.
 *         The caller takes ownership of the returned struct and must free
 *         it with `rcnFreeSourceText()`. Returns a struct with `text` set
 *         to `NULL` on error.
 */
RECKON_EXPORT RcnSourceText rcnMarkLogicalLinesInSourceText(
    RcnTextFormat language,
    RcnSourceText sourceCode
);

/**
 * Frees the previously allocated data of a `RcnSourceText` struct.
 * 
 * Use this deallocation function for `RcnSourceText` structs that were
 * returned by functions of this API that allocate new source text.
 * The struct must not be used after calling this function.
 * 
 * @param source The `RcnSourceText` struct to free. The provided struct
 *               and the `text` field may be `NULL`.
 */
RECKON_EXPORT void rcnFreeSourceText(RcnSourceText* source);

/**
 * Counts the number of hard physical lines in the specified source text.
 * 
 * The count includes all physical lines, including blank lines and comments,
 * not only physical lines of code. The result of this function is therefore
 * independent of any programming language. A physical line count can be
 * computed for every text file, independent of its format.
 * 
 * See header documentation for details on how hard physical lines are
 * defined and for supported encodings.
 *
 * @param source The source text to count physical lines in.
 * @return A `RcnCountResult` struct containing the line count.
 */
RECKON_EXPORT RcnCountResult rcnCountPhysicalLines(RcnSourceText source);

/**
 * Counts the number of words in the specified source text.
 * 
 * A word is a non-zero-length sequence of printable characters delimited
 * by white space. See header documentation for details on supported encodings.
 *
 * @param source The source text to count words in.
 * @return A `RcnCountResult` containing the word count.
 */
RECKON_EXPORT RcnCountResult rcnCountWords(RcnSourceText source);

/**
 * Counts the number of characters in the specified source text.
 *
 * A character is defined as a Unicode code point. This metric includes
 * control characters, like newlines. The returned count therefore includes
 * non-printable characters. See header documentation for details
 * on supported encodings.
 *
 * @param source The source text to count characters in.
 * @return A `RcnCountResult` containing the character count.
 */
RECKON_EXPORT RcnCountResult rcnCountCharacters(RcnSourceText source);

#ifdef __cplusplus
}
#endif
