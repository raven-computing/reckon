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
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "scount.h"

#ifndef RECKON_VERSION
#define RECKON_VERSION "unknown"
#endif

static uint32_t getFormatBit(RcnTextFormat format) {
    return (uint32_t) RECKON_MK_FRMT_OPT(format);
}

static bool equalsSpecifier(
    const char* spec,
    size_t length,
    const char* value
) {
    const size_t valueLen = strlen(value);
    if (length != valueLen) {
        return false;
    }
    for (size_t i = 0; i < length; ++i) {
        const unsigned char lhs = (unsigned char) spec[i];
        const unsigned char rhs = (unsigned char) value[i];
        if (tolower(lhs) != tolower(rhs)) {
            return false;
        }
    }
    return true;
}

// NOLINTBEGIN(readability-function-cognitive-complexity)
static bool parseFormatSpecifier(
    const char* spec,
    size_t length,
    uint32_t* formatBit
) {
    if (!spec || length == 0 || !formatBit) {
        return false;
    }

    if (equalsSpecifier(spec, length, "txt")
        || equalsSpecifier(spec, length, "text")
        || equalsSpecifier(spec, length, "plaintext")
        || equalsSpecifier(spec, length, "plain-text")
        || equalsSpecifier(spec, length, "plain text")
        || equalsSpecifier(spec, length, "unformatted")) {
        *formatBit = getFormatBit(RCN_TEXT_UNFORMATTED);
    } else if (equalsSpecifier(spec, length, "md")
        || equalsSpecifier(spec, length, "markdown")) {
        *formatBit = getFormatBit(RCN_TEXT_MARKDOWN);
    } else if (equalsSpecifier(spec, length, "xml")) {
        *formatBit = getFormatBit(RCN_TEXT_XML);
    } else if (equalsSpecifier(spec, length, "fxml")) {
        *formatBit = getFormatBit(RCN_TEXT_FXML);
    } else if (equalsSpecifier(spec, length, "proto")
        || equalsSpecifier(spec, length, "protobuf")
        || equalsSpecifier(spec, length, "protocol-buffers")
        || equalsSpecifier(spec, length, "protocol buffers")) {
        *formatBit = getFormatBit(RCN_TEXT_PROTOBUF);
    } else if (equalsSpecifier(spec, length, "json")) {
        *formatBit = getFormatBit(RCN_TEXT_JSON);
    } else if (equalsSpecifier(spec, length, "css")) {
        *formatBit = getFormatBit(RCN_TEXT_CSS);
    } else if (equalsSpecifier(spec, length, "html")
        || equalsSpecifier(spec, length, "htm")) {
        *formatBit = getFormatBit(RCN_TEXT_HTML);
    } else if (equalsSpecifier(spec, length, "sql")) {
        *formatBit = getFormatBit(RCN_TEXT_SQL);
    } else if (equalsSpecifier(spec, length, "cmake")
        || equalsSpecifier(spec, length, "cmakelists")
        || equalsSpecifier(spec, length, "cmakelists.txt")) {
        *formatBit = getFormatBit(RCN_TEXT_CMAKE);
    } else if (equalsSpecifier(spec, length, "make")
        || equalsSpecifier(spec, length, "mk")
        || equalsSpecifier(spec, length, "makefile")) {
        *formatBit = getFormatBit(RCN_TEXT_MAKE);
    } else if (equalsSpecifier(spec, length, "yaml")
        || equalsSpecifier(spec, length, "yml")) {
        *formatBit = getFormatBit(RCN_TEXT_YAML);
    } else if (equalsSpecifier(spec, length, "assembly")
        || equalsSpecifier(spec, length, "asm")
        || equalsSpecifier(spec, length, "s")) {
        *formatBit = getFormatBit(RCN_LANG_ASSEMBLY);
    } else if (equalsSpecifier(spec, length, "c")) {
        *formatBit = getFormatBit(RCN_LANG_C);
    } else if (equalsSpecifier(spec, length, "java")) {
        *formatBit = getFormatBit(RCN_LANG_JAVA);
    } else if (equalsSpecifier(spec, length, "python")
        || equalsSpecifier(spec, length, "py")) {
        *formatBit = getFormatBit(RCN_LANG_PYTHON);
    } else if (equalsSpecifier(spec, length, "cpp")
        || equalsSpecifier(spec, length, "c++")
        || equalsSpecifier(spec, length, "cplusplus")
        || equalsSpecifier(spec, length, "cc")
        || equalsSpecifier(spec, length, "cxx")) {
        *formatBit = getFormatBit(RCN_LANG_CPP);
    } else if (equalsSpecifier(spec, length, "cuda")
        || equalsSpecifier(spec, length, "cu")) {
        *formatBit = getFormatBit(RCN_LANG_CUDA);
    } else if (equalsSpecifier(spec, length, "javascript")
        || equalsSpecifier(spec, length, "js")) {
        *formatBit = getFormatBit(RCN_LANG_JAVASCRIPT);
    } else if (equalsSpecifier(spec, length, "typescript")
        || equalsSpecifier(spec, length, "ts")) {
        *formatBit = getFormatBit(RCN_LANG_TYPESCRIPT);
    } else if (equalsSpecifier(spec, length, "r")) {
        *formatBit = getFormatBit(RCN_LANG_R);
    } else if (equalsSpecifier(spec, length, "batch")
        || equalsSpecifier(spec, length, "dos-batch")
        || equalsSpecifier(spec, length, "dos batch")
        || equalsSpecifier(spec, length, "dosbatch")
        || equalsSpecifier(spec, length, "bat")
        || equalsSpecifier(spec, length, "cmd")) {
        *formatBit = getFormatBit(RCN_LANG_BATCH);
    } else if (equalsSpecifier(spec, length, "bash")
        || equalsSpecifier(spec, length, "shell")
        || equalsSpecifier(spec, length, "sh")) {
        *formatBit = getFormatBit(RCN_LANG_BASH);
    } else {
        return false;
    }

    return true;
}
// NOLINTEND(readability-function-cognitive-complexity)

static char* copyInvalidFormatSpecifier(const char* token, size_t length) {
    // Ignoring false positive linter warning
    char* invalidTokenString = malloc(length + 1); // NOLINT
    if (!invalidTokenString) {
        return NULL; // LCOV_EXCL_LINE
    }
    memcpy(invalidTokenString, token, length);
    invalidTokenString[length] = '\0';
    return invalidTokenString;
}

static void parseFormatList(
    const char* value,
    uint32_t* formatMask,
    bool* hasMask,
    char** invalidSpecifier
) {
    if (!value || value[0] == '\0') {
        const char* empty = "";
        *invalidSpecifier = copyInvalidFormatSpecifier(empty, strlen(empty));
        return;
    }

    const char* tokenStart = value;
    while (true) {
        const char* tokenEnd = tokenStart;
        while (*tokenEnd != '\0' && *tokenEnd != ',') {
            ++tokenEnd;
        }

        while (*tokenStart != '\0' && isspace((unsigned char) *tokenStart)) {
            ++tokenStart;
        }
        while (tokenEnd > tokenStart
            && isspace((unsigned char) *(tokenEnd - 1))) {
            --tokenEnd;
        }

        const size_t tokenLength = (size_t) (tokenEnd - tokenStart);
        uint32_t formatBit = 0;
        if (!parseFormatSpecifier(tokenStart, tokenLength, &formatBit)) {
            if (*invalidSpecifier == NULL) {
                *invalidSpecifier = copyInvalidFormatSpecifier(
                    tokenStart,
                    tokenLength
                );
            }
        } else {
            *formatMask |= formatBit;
            *hasMask = true;
        }

        if (*tokenEnd == '\0') {
            break;
        }
        tokenStart = tokenEnd + 1;
    }
}

// NOLINTBEGIN(readability-function-cognitive-complexity)
AppArgs parseArgs(int argc, char** argv) {
    AppArgs args = {0};
    for (int i = 1; i < argc; ++i) {
        const size_t argLen = strlen(argv[i]);
        if (strcmp(argv[i], "--annotate-counts") == 0) {
            args.annotateCounts = true;
        } else if (strcmp(argv[i], "--lines") == 0
                || strcmp(argv[i], "-l") == 0) {
            args.linesOnly = true;
        } else if (strcmp(argv[i], "--total") == 0) {
            args.totalsOnly = true;
        } else if (strcmp(argv[i], "--show-files") == 0) {
            args.showFiles = true;
        } else if (strcmp(argv[i], "--show-all-files") == 0) {
            args.showAllFiles = true;
        } else if (strcmp(argv[i], "--disable-parallelization") == 0) {
            args.disableParallelization = true;
        } else if (strcmp(argv[i], "--include-format") == 0) {
            if (i + 1 >= argc) {
                args.errorMessage = (
                    "Missing argument for option '--include-format'."
                );
                break;
            }
            const char* optionArg = argv[++i];
            parseFormatList(
                optionArg,
                &args.includeFormatMask,
                &args.hasIncludeFormatMask,
                &args.invalidIncludeFormatSpecifier
            );
        } else if (strcmp(argv[i], "--exclude-format") == 0) {
            if (i + 1 >= argc) {
                args.errorMessage = (
                    "Missing argument for option '--exclude-format'."
                );
                break;
            }
            const char* optionArg = argv[++i];
            parseFormatList(
                optionArg,
                &args.excludeFormatMask,
                &args.hasExcludeFormatMask,
                &args.invalidExcludeFormatSpecifier
            );
        } else if (strcmp(argv[i], "--stop-on-error") == 0) {
            args.stopOnError = true;
        } else if (strcmp(argv[i], "--strict") == 0) {
            args.strict = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            args.verbose = true;
        } else if (strcmp(argv[i], "--help") == 0
                || strcmp(argv[i], "-?") == 0) {

            args.help = true;
        } else if (strcmp(argv[i], "--version") == 0) {
            args.version = true;
        } else if (strcmp(argv[i], "-#") == 0) {
            args.versionShort = true;
            args.version = true;
        } else if (strcmp(argv[i], "-") == 0
            || (argLen > 2 && argv[i][0] == '-' && argv[i][1] == '.')) {
            if (args.inputPath == NULL) {
                args.readFromStdin = true;
                args.inputPath = argv[i] + 1;
            } else {
                args.errorMessage = "Multiple input paths specified.";
                break;
            }
        } else if (argv[i][0] == '-') {
            args.indexUnknown = i;
            break;
        } else {
            if (args.inputPath == NULL) {
                args.inputPath = argv[i];
            } else {
                args.errorMessage = "Multiple input paths specified.";
                break;
            }
        }
    }
    if (args.inputPath == NULL && args.errorMessage == NULL) {
        args.errorMessage = "No input path specified.";
    }
    return args;
}
// NOLINTEND(readability-function-cognitive-complexity)

void showUsage(void) {
    logI("Usage: scount [options] <PATH>");
}

void showVersion(AppArgs args) {
    if (args.versionShort) {
        logStdout(RECKON_VERSION);
        logStdout("\n");
        return;
    }
    const char* version = RECKON_VERSION;
    const char* devHint = "";
    char* hyphen = strrchr(version, '-');
    if (hyphen && !strcmp(hyphen, "-dev")) {
        devHint = " (DEVELOPMENT VERSION)";
    }
    logI("scount v%s%s", version, devHint);
    logI("The scount executable is part of the Reckon project.");
    logI("Copyright (C) 2026 Raven Computing");
    logI("This software is licensed under the Apache License, Version 2.0");
    logI(" ");
    logI("Written with <3 by Phil Gaiser");
}

void showHelpText(void) {
    logI("scount: Count source code lines in files.");
    logI(" ");
    showUsage();
    logI(" ");
    logI("Positional Arguments:");
    logI(" ");
    logI("  <PATH>              The path to the input file or directory to process.");
    logI("                      Use '-' to read plain text source input from stdin, or append a");
    logI("                      file extension to indicate a specific source format or language,");
    logI("                      e.g. '-.java' will treat the input provided by stdin as Java code.");
    logI(" ");
    logI("Options:");
    logI(" ");
    logI("  [--annotate-counts] Mark counted logical lines and output the result.");
    logI("                      This option can only be used on a single regular file input.");
    logI(" ");
    logI("  [--disable-parallelization]");
    logI("                      Disable multithreaded file processing.");
    logI("                      By default, scount may process files in parallel when multiple CPU cores");
    logI("                      are available.");
    logI(" ");
    logI("  [--exclude-format <SPEC1[,SPEC2,...]>]");
    logI("                      Exclude the specified text formats/languages.");
    logI("                      Format specifiers are case-insensitive. This option can be repeated.");
    logI(" ");
    logI("  [--include-format <SPEC1[,SPEC2,...]>]");
    logI("                      Include only the specified text formats/languages.");
    logI("                      Format specifiers are case-insensitive. This option can be repeated.");
    logI(" ");
    logI("  [-l|--lines]        Compute and display only line-specific metrics.");
    logI("                      This includes logical and physical lines of code, and hard physical lines.");
    logI(" ");
    logI("  [--total]           Show only the totals summary row in table output.");
    logI("                      This suppresses per-file and per-language detail rows.");
    logI(" ");
    logI("  [--show-files]      Show a table of individual files in the result.");
    logI("                      For large results, a condensed view is shown.");
    logI(" ");
    logI("  [--show-all-files]  Show a table of all individual files in the result.");
    logI("                      All files are listed without any limitation.");
    logI(" ");
    logI("  [--stop-on-error]   Stop processing immediately when an error is encountered.");
    logI(" ");
    logI("  [--strict]          Use strict syntax checking when parsing source code.");
    logI("                      If set, any syntax error will cause failure for the");
    logI("                      affected file. Otherwise, partial errors are tolerated.");
    logI(" ");
    logI("  [--verbose]         Enable verbose output.");
    logI(" ");
    logI("  [-#|--version]      Show program version information.");
    logI(" ");
    logI("  [-?|--help]         Show this help message.");
    logI(" ");
}

bool isInputValid(AppArgs args) {
    if (args.invalidIncludeFormatSpecifier) {
        char* msg = args.invalidIncludeFormatSpecifier;
        logW(
            "Option --include-format contains an invalid format specifier: '%s'",
            msg
        );
        logW("Invalid entries of --include-format option were ignored.");
        free(msg);
        args.invalidIncludeFormatSpecifier = NULL;
    }
    if (args.invalidExcludeFormatSpecifier) {
        char* msg = args.invalidExcludeFormatSpecifier;
        logW(
            "Option --exclude-format contains an invalid format specifier: '%s'",
            msg
        );
        logW("Invalid entries of --exclude-format option were ignored.");
        free(msg);
        args.invalidExcludeFormatSpecifier = NULL;
    }
    if (args.totalsOnly && args.showFiles) {
        logW("Option --show-files cannot be combined with --total option");
    }
    if (args.totalsOnly && args.showAllFiles) {
        logW("Option --show-all-files cannot be combined with --total option");
    }
    return (
        args.errorMessage == NULL
        && args.indexUnknown == 0
        && args.inputPath != NULL
    );
}
