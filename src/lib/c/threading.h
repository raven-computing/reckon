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
 * Internal utilities for multi-threading.
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "reckon/reckon.h"
#include "evaluation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ThreadHandle {
    void* nativeHandle;
} ThreadHandle;

typedef struct ThreadControl {
    void* nativeMutex;
    bool abortRequested;
} ThreadControl;

typedef struct ThreadWork {
    RcnCountStatistics* stats;
    RcnStatOptions options;
    Slice slice;
    ThreadControl* control;
} ThreadWork;

typedef void (*ThreadRoutine)(ThreadWork* arg);

bool initThreadControl(ThreadControl* control);

void deinitThreadControl(ThreadControl* control);

bool shouldAbortRange(ThreadControl* control);

void requestAbortRange(ThreadControl* control);

size_t getSystemConcurrency(void);

bool createThread(
    ThreadHandle* handle,
    ThreadRoutine routine,
    ThreadWork* arg
);

void joinThread(ThreadHandle* handle);

bool initThreadMutex(void** mutex);

void deinitThreadMutex(void* mutex);

void lockThreadMutex(void* mutex);

void unlockThreadMutex(void* mutex);

#ifdef __cplusplus
}
#endif
