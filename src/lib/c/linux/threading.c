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

#ifdef __linux__

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "threading.h"

typedef struct RcnThreadArg {
    ThreadRoutine routine;
    ThreadWork* arg;
} RcnThreadArg;

static void* rcnRun(void* arg) {
    RcnThreadArg* feeder = (RcnThreadArg*) arg;
    ThreadRoutine routine = feeder->routine;
    ThreadWork* threadArg = feeder->arg;
    free(feeder);
    routine(threadArg);
    return NULL;
}

bool initThreadMutex(void** mutex) {
    pthread_mutex_t* nativeMutex = malloc(sizeof(pthread_mutex_t));
    if (!nativeMutex) {
        return false;
    }
    if (pthread_mutex_init(nativeMutex, NULL) != 0) {
        free(nativeMutex);
        return false;
    }
    *mutex = nativeMutex;
    return true;
}

void deinitThreadMutex(void* mutex) {
    pthread_mutex_t* nativeMutex = mutex;
    if (!nativeMutex) {
        return;
    }
    (void) pthread_mutex_destroy(nativeMutex);
    free(nativeMutex);
}

void lockThreadMutex(void* mutex) {
    (void) pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void unlockThreadMutex(void* mutex) {
    (void) pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

size_t getSystemConcurrency(void) {
    long processorCount = sysconf(_SC_NPROCESSORS_ONLN);
    if (processorCount <= 0) {
        return 1;
    }
    return (size_t) processorCount;
}

bool createThread(
    ThreadHandle* handle,
    ThreadRoutine routine,
    ThreadWork* arg
) {
    pthread_t* nativeThread = malloc(sizeof(pthread_t));
    RcnThreadArg* feeder = malloc(sizeof(RcnThreadArg));
    if (!nativeThread || !feeder) {
        free(nativeThread);
        free(feeder);
        return false;
    }
    feeder->routine = routine;
    feeder->arg = arg;
    if (pthread_create(nativeThread, NULL, rcnRun, feeder) != 0) {
        free(feeder);
        free(nativeThread);
        return false;
    }
    handle->nativeHandle = nativeThread;
    return true;
}

void joinThread(ThreadHandle* handle) {
    pthread_t* nativeThread = (pthread_t*) handle->nativeHandle;
    if (!nativeThread) {
        return;
    }
    (void) pthread_join(*nativeThread, NULL);
    free(nativeThread);
    handle->nativeHandle = NULL;
}

#endif // __linux__
