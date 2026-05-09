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

#include <stdio.h>

#include "scount.h"

int main(int argc, char** argv) {
    AppArgs args = parseArgs(argc, argv);
    initLogging(
        stdout,
        stderr,
        args.verbose ? LOG_LEVEL_VERBOSE : LOG_LEVEL_INFO
    );
    if (args.help) {
        showHelpText();
        return APP_EXIT_SUCCESS;
    }
    if (args.version) {
        showVersion(args);
        return APP_EXIT_SUCCESS;
    }
    if (!isInputValid(args)) {
        if (args.indexUnknown) {
            logE("Unknown option: '%s'", argv[args.indexUnknown]);
            return APP_EXIT_INVALID_ARGUMENT;
        }
        if (argc == 1) {
            logW("No input path specified.");
            showUsage();
            logI("Try 'scount --help' for more information.");
        } else if (args.errorMessage) {
            logE("%s", args.errorMessage);
        }
        return APP_EXIT_INVALID_ARGUMENT;
    }
    char* tempInputPath = NULL;
    if (args.readFromStdin) {
        tempInputPath = createTempInputFileFromStdin(args.inputPath);
        if (!tempInputPath) {
            logE("Failed to read source input from stdin.");
            return APP_EXIT_PROG_IO_ERROR;
        }
        args.inputPath = tempInputPath;
    }
    ExitStatus status = APP_EXIT_UNSPECIFIED_ERROR;
    if (args.annotateCounts) {
        status = outputAnnotatedSource(args);
    } else {
        status = outputStatistics(args);
    }
    removeTempInputFile(tempInputPath);
    if (LOG_IO_ERROR_DETECTED) {
        status = APP_EXIT_PROG_IO_ERROR;
    }
    return status;
}
