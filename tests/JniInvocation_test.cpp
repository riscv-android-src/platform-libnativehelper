/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../JniInvocation-priv.h"

#include <gtest/gtest.h>

#include "string.h"

// Local definition for PROPERTY_VALUE_MAX.
// NB Only used to create buffer of expected size, buffer not written to outside of this tests.
static const size_t kPropertyValueMax = 92;

static const char* kTestNonNull = "libartd.so";
static const char* kTestNonNull2 = "libartd2.so";
static const char* kExpected = "libart.so";

static int IsDebuggableAlways() {
    return 1;
}

static int IsDebuggableNever() {
    return 0;
}

static int GetPropertyForTest(char* buffer) {
    strcpy(buffer, kTestNonNull2);
    return strlen(buffer);
}

TEST(JNIInvocation, Debuggable) {
    // On Android, when debuggable property is true, the invocation library can be
    // overridden.
#ifdef __ANDROID__
    char buffer[kPropertyValueMax];
    const char* result =
        JniInvocationGetLibraryWith(nullptr, buffer, IsDebuggableAlways, GetPropertyForTest);
    EXPECT_STREQ(result, kTestNonNull2);

    result =
        JniInvocationGetLibraryWith(kTestNonNull, buffer, IsDebuggableAlways, GetPropertyForTest);
    EXPECT_STREQ(result, kTestNonNull);
#else  // __ANDROID__
    // On host, the invocation can always be overridden. The arguments |buffer|,
    // |is_debuggable| and |get_library_system_property| are ignored by JniInvocationGetLibraryWith.
    const char* result =
        JniInvocationGetLibraryWith(nullptr, nullptr, IsDebuggableAlways, GetPropertyForTest);
    EXPECT_STREQ(result, kExpected);
    result =
        JniInvocationGetLibraryWith(kTestNonNull, nullptr, IsDebuggableAlways, GetPropertyForTest);
    EXPECT_STREQ(result, kTestNonNull);
#endif  // __ANDROID__
}

TEST(JNIInvocation, NonDebuggable) {
#ifdef __ANDROID__
    // On Android, when debuggable property is false, the invocation library provided is
    // irrelevant, the default "libart.so" is always used.
    char buffer[kPropertyValueMax];
    const char* result = JniInvocationGetLibraryWith(nullptr, buffer, IsDebuggableNever, nullptr);
    EXPECT_STREQ(result, kExpected);

    result = JniInvocationGetLibraryWith(kTestNonNull, buffer, IsDebuggableNever, nullptr);
    EXPECT_STREQ(result, kExpected);

    result = JniInvocationGetLibraryWith(nullptr, buffer, IsDebuggableNever, GetPropertyForTest);
    EXPECT_STREQ(result, kExpected);

    result =
        JniInvocationGetLibraryWith(kTestNonNull, buffer, IsDebuggableNever, GetPropertyForTest);
    EXPECT_STREQ(result, kExpected);
#else  // __ANDROID__
    // Host does not have a debuggable property, the invocation library can always be overridden.
    char buffer[kPropertyValueMax];
    const char* result = JniInvocationGetLibraryWith(nullptr, buffer, IsDebuggableNever, nullptr);
    EXPECT_STREQ(result, kExpected);

    result = JniInvocationGetLibraryWith(kTestNonNull, buffer, IsDebuggableNever, nullptr);
    EXPECT_STREQ(result, kTestNonNull);

    result = JniInvocationGetLibraryWith(nullptr, buffer, IsDebuggableNever, GetPropertyForTest);
    EXPECT_STREQ(result, kExpected);

    result =
        JniInvocationGetLibraryWith(kTestNonNull, buffer, IsDebuggableNever, GetPropertyForTest);
    EXPECT_STREQ(result, kTestNonNull);
#endif  // __ANDROID__
}
