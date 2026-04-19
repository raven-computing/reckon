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
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#define TEMP_CLOSE _close
#define TEMP_UNLINK _unlink
#else
#include <unistd.h>
#define TEMP_CLOSE close
#define TEMP_UNLINK unlink
#endif

#include "scount.h"

// LCOV_EXCL_START
static char* cleanup(char* tmpPath, char* tmpPathMk, int fd, FILE* tmpFile) {
    if (tmpFile) {
        (void) fclose(tmpFile);
        fd = -1;
    }
    if (fd >= 0) {
        TEMP_CLOSE(fd);
    }
    if (tmpPathMk) {
        TEMP_UNLINK(tmpPathMk);
    }
    if (tmpPath) {
        free(tmpPath);
    }
    return NULL;
}
// LCOV_EXCL_STOP

#ifdef _WIN32

static char* createTempFileImpl(const char* suffix, int* outFd) {
    char tempDir[MAX_PATH + 1];
    const DWORD tempDirLen = GetTempPathA((DWORD) sizeof(tempDir), tempDir);
    if (tempDirLen == 0 || tempDirLen >= sizeof(tempDir)) {
        return NULL;
    }

    for (unsigned int i = 0; i < 256; ++i) {
        const unsigned int unique = (
            (unsigned int) GetTickCount()
            ^ (unsigned int) GetCurrentProcessId()
            ^ (i * 2654435761u)  // Knuth
        );
        const size_t pathLen = (
            strlen(tempDir) + strlen("scount-stdin-") + 8 + strlen(suffix) + 1
        );
        char* pathTemplate = malloc(pathLen);
        if (!pathTemplate) {
            return NULL;
        }
        (void) snprintf(
            pathTemplate,
            pathLen,
            "%sscount-stdin-%08x%s",
            tempDir,
            unique,
            suffix
        );
        const int fd = _open(
            pathTemplate,
            _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE
        );
        if (fd >= 0) {
            *outFd = fd;
            return pathTemplate;
        }

        free(pathTemplate);
        if (errno != EEXIST) {
            return NULL;
        }
    }

    return NULL;
}

static FILE* openFileImpl(int fd) {
    return _fdopen(fd, "wb");
}

#else

static char* createTempFileImpl(const char* suffix, int* outFd) {
    const char* const prefix = "/tmp/scount-stdin-";
    const char* const templ = "XXXXXX";
    const size_t templateLen = (
        strlen(prefix) + strlen(templ) + strlen(suffix) + 1
    );

    char* pathTemplate = malloc(templateLen);
    if (!pathTemplate) {
        return NULL; // LCOV_EXCL_LINE
    }
    const int nWritten = snprintf(
        pathTemplate,
        templateLen,
        "%s%s%s",
        prefix,
        templ,
        suffix
    );
    if (nWritten < 0 || (size_t) nWritten >= templateLen) {
        free(pathTemplate);
        return NULL; // LCOV_EXCL_LINE
    }

    *outFd = mkstemps(pathTemplate, (int) strlen(suffix));
    if (*outFd < 0) {
        free(pathTemplate);
        return NULL; // LCOV_EXCL_LINE
    }

    return pathTemplate;
}

static FILE* openFileImpl(int fd) {
    return fdopen(fd, "wb");
}

#endif

char* createTempInputFileFromStdin(const char* extension) {
    const char* const suffix = (
        extension && strcmp(extension, "") != 0
        ? extension
        : ".txt"
    );
    char* pathTemplate = NULL;
    int fd = -1;

    pathTemplate = createTempFileImpl(suffix, &fd);
    if (!pathTemplate || fd < 0) {
        return cleanup(pathTemplate, NULL, fd, NULL); // LCOV_EXCL_LINE
    }

    FILE* out = openFileImpl(fd);
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
    if (TEMP_UNLINK(path) != 0) {
        logW("Failed to remove temporary input file '%s'", path); // LCOV_EXCL_LINE
    }
    free(path);
}
