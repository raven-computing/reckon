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

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "reckon/reckon.h"

#if defined(__clang__)
#define ATTR_LOG_FORMAT __attribute__((format(printf, 1, 2)))
#elif defined(__GNUC__)
#define ATTR_LOG_FORMAT __attribute__((format(gnu_printf, 1, 2)))
#else
#define ATTR_LOG_FORMAT
#endif

/**
 * Enumeration of all exit status codes of scount.
 */
typedef enum ExitStatus {
    APP_EXIT_SUCCESS = 0,
    APP_EXIT_INVALID_ARGUMENT = 1,
    APP_EXIT_INVALID_INPUT = 2,
    APP_EXIT_NOTHING_PROCESSED = 3,
    APP_EXIT_PROG_IO_ERROR = 4,
    APP_EXIT_UNSPECIFIED_ERROR = 126
} ExitStatus;

/**
 * Structure holding all parsed application arguments.
 */
typedef struct AppArgs {
    char* inputPath;     // The input `<PATH>` to process
    char* errorMessage;  // Error message in case of invalid input
    int indexUnknown;    // Index into `argv` when unknown arg found, or zero
    bool annotateCounts; // Option: `--annotate-counts`
    bool stopOnError;    // Option: `--stop-on-error`
    bool verbose;        // Option: `--verbose`
    bool version;        // Option: `-#|--version`
    bool versionShort;   // Option: `-#`
    bool help;           // Option: `-?`|`--help`
} AppArgs;

/**
 * Enumeration of all log levels.
 */
typedef enum LogLevel {
    LOG_LEVEL_DISABLED,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_VERBOSE
} LogLevel;

/**
 * Current log level for the application.
 */
extern LogLevel LOG_LEVEL;

/**
 * Output stream for non-error message logging.
 */
extern FILE* LOG_STREAM_OUT;

/**
 * Output stream for error message logging.
 */
extern FILE* LOG_STREAM_ERR;

/**
 * Whether an error was detected during logging.
 */
extern bool LOG_IO_ERROR_DETECTED;

/**
 * Text buffer of a null-terminated formatted string used for printing.
 */
typedef struct PrintBuffer {
    char* text;
    size_t size;
    size_t capacity;
} PrintBuffer;

/**
 * Parses command line arguments.
 *
 * @param argc The argument count given to the application.
 * @param argv The argument vector given to the application.
 * @return An `AppArgs` struct containing the parsed arguments.
 */
AppArgs parseArgs(int argc, char** argv);

/**
 * Displays usage information for the application on stdout.
 */
void showUsage(void);

/**
 * Displays version information for the application on stdout.
 */
void showVersion(AppArgs args);

/**
 * Displays help text for the application on stdout.
 */
void showHelpText(void);

/**
 * Validates the input arguments.
 * 
 * Returns `true` if the parsed application arguments contains
 * no error and the input can be further used, `false` otherwise.
 */
bool isInputValid(AppArgs args);

/**
 * Processes the input path and shows statistics on stdout.
 * 
 * This is the main operation function of scount handling most user inputs.
 *
 * @param args The parsed application arguments.
 * @return The exit status of the operation.
 */
ExitStatus outputStatistics(AppArgs args);

/**
 * Processes the input path and shows annotated source code on stdout.
 * 
 * @param args The parsed application arguments.
 * @return The exit status of the operation.
 */
ExitStatus outputAnnotatedSource(AppArgs args);

/**
 * Creates textual result output for processed statistics when the
 * given input is a single regular file.
 * 
 * @param stats The statistics containing one processed file.
 * @return A `PrintBuffer` containing the formatted result.
 *         The caller must free the text buffer.
 */
PrintBuffer printResultSingle(const RcnCountStatistics* stats);

/**
 * Creates textual result output for processed statistics when the
 * given input is a directory possibly containing multiple files.
 * 
 * @param path The path to the input directory.
 * @param stats The statistics for the files.
 * @return A `PrintBuffer` containing the formatted result.
 *         The caller must free the text buffer.
 */
PrintBuffer printResultsMultiple(
    const char* path,
    const RcnCountStatistics* stats
);

/**
 * Logs a message to stdout.
 * The string is not further formatted and dumped to stdout as is.
 * 
 * @param text The string to log.
 */
void logStdout(const char* text);

/**
 * Logs a formatted message with ERROR level.
 *
 * @param format The format string.
 * @param ... The values to format.
 */
void logE(const char* format, ...) ATTR_LOG_FORMAT;

/**
 * Logs a formatted message with WARNING level.
 *
 * @param format The format string.
 * @param ... The values to format.
 */
void logW(const char* format, ...) ATTR_LOG_FORMAT;

/**
 * Logs a formatted message with INFO level.
 *
 * @param format The format string.
 * @param ... The values to format.
 */
void logI(const char* format, ...) ATTR_LOG_FORMAT;

/**
 * Logs a formatted message with VERBOSE level.
 *
 * @param format The format string.
 * @param ... The values to format.
 */
void logV(const char* format, ...) ATTR_LOG_FORMAT;
