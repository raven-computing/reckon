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

#include <stdbool.h>
#include <string.h>

#include "scount.h"

#ifndef RECKON_VERSION
#define RECKON_VERSION "unknown"
#endif

AppArgs parseArgs(int argc, char** argv) {
    AppArgs args = {0};
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--annotate-counts") == 0) {
            args.annotateCounts = true;
        } else if (strcmp(argv[i], "--stop-on-error") == 0) {
            args.stopOnError = true;
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
    if (args.inputPath == NULL) {
        args.errorMessage = "No input path specified.";
    }
    return args;
}

void showUsage(void) {
    logI("Usage: scount [--verbose] [--annotate-counts] [--stop-on-error] <PATH>");
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
    logI(" ");
    logI("Options:");
    logI(" ");
    logI("  [--annotate-counts] Mark counted logical lines and output the result.");
    logI("                      This option can only be used on a single file input.");
    logI(" ");
    logI("  [--stop-on-error]   Stop processing on first error.");
    logI(" ");
    logI("  [--verbose]         Enable verbose output.");
    logI(" ");
    logI("  [-#|--version]      Show program version information.");
    logI(" ");
    logI("  [-?|--help]         Show this help message.");
    logI(" ");
}

bool isInputValid(AppArgs args) {
    return (
        args.errorMessage == NULL
        && args.indexUnknown == 0
        && args.inputPath != NULL
    );
}
