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

#include "unity.h"

#include "threading.h"

void setUp(void) { }

void tearDown(void) { }

void testThreadControlLifecycleWorksOnMainThread(void) {
    ThreadControl control;
    bool initOk = initThreadControl(&control);

    TEST_ASSERT_TRUE(initOk);
    TEST_ASSERT_NOT_NULL(control.mutex);
    bool shouldAbort = shouldAbortRange(&control);
    TEST_ASSERT_FALSE(shouldAbort);

    requestAbortRange(&control);
    shouldAbort = shouldAbortRange(&control);
    TEST_ASSERT_TRUE(shouldAbort);

    deinitThreadControl(&control);
    TEST_ASSERT_NULL(control.mutex);
}

void testThreadControlAbortFunctionsAcceptNullControl(void) {
    bool shouldAbort = shouldAbortRange(NULL);
    TEST_ASSERT_FALSE(shouldAbort);
    requestAbortRange(NULL);
}

void testThreadMutexLifecycleWorksOnMainThread(void) {
    RCN_NATIVE_HANDLE mutex = NULL;
    bool initOk = initThreadMutex(&mutex);

    TEST_ASSERT_TRUE(initOk);
    TEST_ASSERT_NOT_NULL(mutex);

    lockThread(mutex);
    unlockThread(mutex);
    deinitThreadMutex(mutex);
}

void testThreadMutexDeinitAcceptsNullHandle(void) {
    deinitThreadMutex(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testThreadControlLifecycleWorksOnMainThread);
    RUN_TEST(testThreadControlAbortFunctionsAcceptNullControl);
    RUN_TEST(testThreadMutexLifecycleWorksOnMainThread);
    RUN_TEST(testThreadMutexDeinitAcceptsNullHandle);
    return UNITY_END();
}
