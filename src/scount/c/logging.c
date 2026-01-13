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
#include <stdarg.h>

#include "scount.h"

LogLevel LOG_LEVEL = LOG_LEVEL_DISABLED;

FILE* LOG_STREAM_OUT = NULL;
FILE* LOG_STREAM_ERR = NULL;
bool LOG_IO_ERROR_DETECTED = false;

void logStdout(const char* text) {
    if (LOG_LEVEL == LOG_LEVEL_DISABLED) {
        return;
    }
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || printf("%s", text) < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fflush(stdout) != 0
    );
}

void logE(const char* format, ...) {
    if (LOG_LEVEL < LOG_LEVEL_ERROR || LOG_STREAM_ERR == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || vfprintf(LOG_STREAM_ERR, format, args) < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fprintf(LOG_STREAM_ERR, "\n") < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fflush(LOG_STREAM_ERR) != 0
    );
    va_end(args);
}

void logW(const char* format, ...) {
    if (LOG_LEVEL < LOG_LEVEL_WARNING || LOG_STREAM_OUT == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fprintf(LOG_STREAM_OUT, "Warning: ") < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || vfprintf(LOG_STREAM_OUT, format, args) < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fprintf(LOG_STREAM_OUT, "\n") < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fflush(LOG_STREAM_OUT) != 0
    );
    va_end(args);
}

void logI(const char* format, ...) {
    if (LOG_LEVEL < LOG_LEVEL_INFO || LOG_STREAM_OUT == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || vfprintf(LOG_STREAM_OUT, format, args) < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fprintf(LOG_STREAM_OUT, "\n") < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fflush(LOG_STREAM_OUT) != 0
    );
    va_end(args);
}

void logV(const char* format, ...) {
    if (LOG_LEVEL < LOG_LEVEL_VERBOSE || LOG_STREAM_OUT == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || vfprintf(LOG_STREAM_OUT, format, args) < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fprintf(LOG_STREAM_OUT, "\n") < 0
    );
    LOG_IO_ERROR_DETECTED = (
        LOG_IO_ERROR_DETECTED || fflush(LOG_STREAM_OUT) != 0
    );
    va_end(args);
}
