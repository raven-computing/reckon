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

/**
 * Represents a native handle to some platform-specific object,
 * like a thread or synchronization primitive.
 */
#define RCN_NATIVE_HANDLE void*

/**
 * An opaque handle representing a thread.
 * The underlying implementation is platform-specific.
 */
typedef struct ThreadHandle {
    RCN_NATIVE_HANDLE handle;
} ThreadHandle;

/**
 * A control structure for managing thread execution, including
 * synchronization and abort requests.
 */
typedef struct ThreadControl {
    RCN_NATIVE_HANDLE mutex;
    bool abortRequested;
} ThreadControl;

/**
 * A structure encapsulating the work to be performed by a single thread.
 * The `stats` field points to the `RcnCountStatistics` structure that is
 * shared by potentially multiple worker threads. It will be updated with
 * the computed results of a thread's work.
 */
typedef struct ThreadWork {
    RcnCountStatistics* stats;
    RcnStatOptions options;
    Slice slice;
    ThreadControl* control;
} ThreadWork;

/**
 * A function pointer type representing the routine executed by a thread.
 */
typedef void (*ThreadRoutine)(ThreadWork* arg);

/**
 * Initializes the specified `ThreadControl` struct, setting up the necessary
 * synchronization primitives and preparing it for use.
 */
bool initThreadControl(ThreadControl* control);

/**
 * Deinitializes the specified `ThreadControl` struct, releasing any resources
 * associated with it.
 */
void deinitThreadControl(ThreadControl* control);

/**
 * Indicates whether the current thread should abort its work.
 */
bool shouldAbortRange(ThreadControl* control);

/**
 * Requests that the current thread abort its work.
 */
void requestAbortRange(ThreadControl* control);

/**
 * Indicates the number of threads that can be used by the system
 * to parallelize work.
 */
size_t getSystemConcurrency(void);

/**
 * Creates a new thread that executes the specified routine with the
 * given argument. The routine is executed immediately upon thread creation.
 * Returns `true` if the thread was successfully created and started.
 * Returns `false` if an error occurred and the thread was not created or
 * started as a result of that.
 */
bool createThread(
    ThreadHandle* handle,
    ThreadRoutine routine,
    ThreadWork* arg
);

/**
 * Waits for the specified thread to complete its execution.
 */
void joinThread(ThreadHandle* handle);

/**
 * Initializes a thread mutex, allocating and setting up the necessary
 * synchronization primitives.
 * Returns `true` if the mutex was successfully initialized, `false` otherwise.
 */
bool initThreadMutex(RCN_NATIVE_HANDLE* mutex);

/**
 * Deinitializes a thread mutex, releasing any resources associated with it.
 */
void deinitThreadMutex(RCN_NATIVE_HANDLE mutex);

/**
 * Locks the specified thread mutex, blocking the calling thread until
 * the mutex becomes available.
 */
void lockThreadMutex(RCN_NATIVE_HANDLE mutex);

/**
 * Unlocks the specified thread mutex, allowing other threads to acquire it.
 */
void unlockThreadMutex(RCN_NATIVE_HANDLE mutex);

#ifdef __cplusplus
}
#endif
