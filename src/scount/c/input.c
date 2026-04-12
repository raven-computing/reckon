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
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "scount.h"

// LCOV_EXCL_START
static char* cleanup(char* tmpPath, char* tmpPathMk, int fd, FILE* tmpFile) {
    if (tmpFile) {
        (void) fclose(tmpFile);
    }
    if (fd >= 0) {
        close(fd);
    }
    if (tmpPathMk) {
        unlink(tmpPathMk);
    }
    if (tmpPath) {
        free(tmpPath);
    }
    return NULL;
}
// LCOV_EXCL_STOP

char* createTempInputFileFromStdin(const char* extension) {
    const char* const prefix = "/tmp/scount-stdin-";
    const char* const templ = "XXXXXX";
    const char* const suffix = (
        extension && strcmp(extension, "") != 0
        ? extension
        : ".txt"
    );
    const size_t templateLen = strlen(prefix) + strlen(templ) + strlen(suffix) + 1;

    char* pathTemplate = malloc(templateLen);
    if (!pathTemplate) {
        return NULL; // LCOV_EXCL_LINE
    }
    strncpy(pathTemplate, prefix, templateLen - 1);
    strncat(pathTemplate, templ, templateLen - strlen(pathTemplate) - 1);
    strncat(pathTemplate, suffix, templateLen - strlen(pathTemplate) - 1);
    pathTemplate[templateLen - 1] = '\0';

    int fd = mkstemps(pathTemplate, (int) strlen(suffix));
    if (fd < 0) {
        return cleanup(pathTemplate, NULL, fd, NULL); // LCOV_EXCL_LINE
    }
    FILE* out = fdopen(fd, "wb");
    if (!out) {
        return cleanup(pathTemplate, pathTemplate, fd, NULL); // LCOV_EXCL_LINE
    }

    char buffer[2048];
    while (true) {
        const size_t readCount = fread(buffer, 1, sizeof(buffer), stdin);
        if (readCount > 0) {
            if (fwrite(buffer, 1, readCount, out) != readCount) {
                return cleanup(pathTemplate, pathTemplate, fd, out); // LCOV_EXCL_LINE
            }
        }
        if (readCount < sizeof(buffer)) {
            if (ferror(stdin)) {
                return cleanup(pathTemplate, pathTemplate, fd, out); // LCOV_EXCL_LINE
            }
            break;
        }
    }

    if (fclose(out) != 0) {
        return cleanup(pathTemplate, pathTemplate, -1, NULL); // LCOV_EXCL_LINE
    }

    return pathTemplate;
}

void removeTempInputFile(char* path) {
    if (!path) {
        return;
    }
    if (unlink(path) != 0) {
        logW("Failed to remove temporary input file '%s'", path); // LCOV_EXCL_LINE
    }
    free(path);
}
