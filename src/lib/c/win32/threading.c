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

#ifdef _WIN32

#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

#include "threading.h"

typedef struct RcnThreadArg {
    ThreadRoutine routine;
    ThreadWork* arg;
} RcnThreadArg;

static DWORD WINAPI rcnRun(LPVOID arg) {
    RcnThreadArg* feeder = arg;
    ThreadRoutine routine = feeder->routine;
    ThreadWork* threadArg = feeder->arg;
    free(feeder);
    routine(threadArg);
    return 0;
}

bool initThreadMutex(void** mutex) {
    CRITICAL_SECTION* nativeMutex = malloc(sizeof(CRITICAL_SECTION));
    if (!nativeMutex) {
        return false;
    }
    InitializeCriticalSection(nativeMutex);
    *mutex = nativeMutex;
    return true;
}

void deinitThreadMutex(void* mutex) {
    CRITICAL_SECTION* nativeMutex = mutex;
    if (!nativeMutex) {
        return;
    }
    DeleteCriticalSection(nativeMutex);
    free(nativeMutex);
}

void lockThreadMutex(void* mutex) {
    EnterCriticalSection((CRITICAL_SECTION*) mutex);
}

void unlockThreadMutex(void* mutex) {
    LeaveCriticalSection((CRITICAL_SECTION*) mutex);
}

size_t getSystemConcurrency(void) {
    DWORD processorCount = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
    if (processorCount == 0) {
        return 1;
    }
    if (processorCount > MAXIMUM_WAIT_OBJECTS) {
        return MAXIMUM_WAIT_OBJECTS;
    }
    return (size_t) processorCount;
}

bool createThread(
    ThreadHandle* handle,
    ThreadRoutine routine,
    ThreadWork* arg
) {
    RcnThreadArg* feeder = malloc(sizeof(RcnThreadArg));
    if (!feeder) {
        return false;
    }
    feeder->routine = routine;
    feeder->arg = arg;
    HANDLE nativeThread = CreateThread(
        NULL,
        0,
        rcnRun,
        feeder,
        0,
        NULL
    );
    if (!nativeThread) {
        free(feeder);
        return false;
    }
    handle->nativeHandle = nativeThread;
    return true;
}

void joinThread(ThreadHandle* handle) {
    HANDLE nativeThread = (HANDLE) handle->nativeHandle;
    if (!nativeThread) {
        return;
    }
    WaitForSingleObject(nativeThread, INFINITE);
    CloseHandle(nativeThread);
    handle->nativeHandle = NULL;
}

#endif // _WIN32
