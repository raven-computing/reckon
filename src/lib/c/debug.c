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
#include <stdatomic.h>
#include <string.h>
#include <assert.h>

#include "tree_sitter/api.h"

#include "reckon/reckon.h"
#include "evaluation.h"
#include "threading.h"

#ifdef RECKON_DEBUG

static atomic_flag DEBUG_INIT_LOCK = ATOMIC_FLAG_INIT;
static atomic_bool DEBUG_INIT_DONE = false;

static bool DEBUG_MODE_ENABLED = false;
static bool DEBUG_MUTEX_INITIALIZED = false;
static RCN_NATIVE_HANDLE DEBUG_OUTPUT_MUTEX = NULL;

static void deinitDebugLogging(void) {
    if (DEBUG_OUTPUT_MUTEX) {
        deinitThreadMutex(DEBUG_OUTPUT_MUTEX);
        DEBUG_OUTPUT_MUTEX = NULL;
    }
}

static bool ensureDebugLoggingInitialized(void) {
    if (DEBUG_INIT_DONE) {
        return DEBUG_MUTEX_INITIALIZED;
    }

    while (atomic_flag_test_and_set(&DEBUG_INIT_LOCK)) { }

    if (!DEBUG_INIT_DONE) {
        const char* envVarValue = getenv(RECKON_ENV_VAR_DEBUG);
        if (envVarValue) {
            if (strcmp(envVarValue, "1") == 0) {
                DEBUG_MODE_ENABLED = true;
            } else if (strcmp(envVarValue, "0") != 0) {
                (void) fprintf(
                    stderr,
                    "[WARN] Invalid value for environment variable "
                    "'%s'. Expected \"0\" or \"1\" but found \"%s\". "
                    "Disabling debug logging.\n",
                    RECKON_ENV_VAR_DEBUG,
                    envVarValue
                );
            }
        }

        DEBUG_MUTEX_INITIALIZED = initThreadMutex(&DEBUG_OUTPUT_MUTEX);
        if (DEBUG_MUTEX_INITIALIZED) {
            (void) atexit(deinitDebugLogging);
        } else if (DEBUG_MODE_ENABLED) {
            (void) fprintf(
                stderr,
                "[WARN] Failed to initialize debug output mutex. "
                "Disabling debug logging.\n"
            );
            DEBUG_MODE_ENABLED = false;
        }

        DEBUG_INIT_DONE = true;
    }

    atomic_flag_clear(&DEBUG_INIT_LOCK);
    return DEBUG_MUTEX_INITIALIZED;
}

static bool isDebugModeEnabled(void) {
    if (!ensureDebugLoggingInitialized()) {
        return false;
    }
    return DEBUG_MODE_ENABLED;
}

void logDebugNode(TSNode node) {
    if (isDebugModeEnabled()) {
        const TSSymbol symbolIdentifier = ts_node_grammar_symbol(node);
        const char* symbolName = ts_node_grammar_type(node);
        assert(symbolName != NULL);
        const char* errorMessage = "";
        if (strcmp(symbolName, "\n") == 0) {
            symbolName = "\\n";
        }
        if (ts_node_is_error(node)) {
            errorMessage = " [ERROR]: Invalid";
        } else if (ts_node_is_missing(node)) {
            errorMessage = " [ERROR]: Missing";
        }
        const TSPoint point = ts_node_start_point(node);
        lockThread(DEBUG_OUTPUT_MUTEX);
        printf(
            "[DEBUG] Line: %6u  Col: %6u  Node: %-32s (%u)%s\n",
            (unsigned) (point.row + 1),
            (unsigned) (point.column + 1),
            symbolName,
            (unsigned) symbolIdentifier,
            errorMessage
        );
        unlockThread(DEBUG_OUTPUT_MUTEX);
    }
}

void logDebugMessage(const char* message) {
    assert(message != NULL);
    if (isDebugModeEnabled()) {
        lockThread(DEBUG_OUTPUT_MUTEX);
        printf("[DEBUG] %s\n", message);
        unlockThread(DEBUG_OUTPUT_MUTEX);
    }
}

#endif
