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

#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

#include "threading.h"

bool initThreadControl(ThreadControl* control) {
    atomic_init(&control->abortRequested, false);
    control->mutex = NULL;
    return initThreadMutex(&control->mutex);
}

void deinitThreadControl(ThreadControl* control) {
    deinitThreadMutex(control->mutex);
    control->mutex = NULL;
}

bool shouldAbortRange(ThreadControl* control) {
    bool abortRequested = false;
    if (control) {
        abortRequested = atomic_load_explicit(
            &control->abortRequested,
            memory_order_relaxed
        );
    }
    return abortRequested;
}

void requestAbortRange(ThreadControl* control) {
    if (control) {
        atomic_store_explicit(
            &control->abortRequested,
            true,
            memory_order_relaxed
        );
    }
}
