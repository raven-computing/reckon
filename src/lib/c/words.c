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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#include "reckon/reckon.h"

RcnCountResult rcnCountWords(RcnSourceText source) {
    RcnCountResult result = {0};
    if (source.size == 0) {
        result.state.ok = true;
        result.state.errorCode = RCN_ERR_NONE;
        return result;
    }
    if (!source.text) {
        result.state.errorCode = RCN_ERR_INVALID_INPUT;
        result.state.errorMessage = "Text input must not be NULL";
        return result;
    }
    if (source.size > UINT32_MAX) {
        result.state.errorCode = RCN_ERR_INPUT_TOO_LARGE;
        result.state.errorMessage = "Input exceeds maximum supported size";
        return result;
    }

    bool inWord = false;
    for (size_t i = 0; i < source.size; ++i) {
        unsigned char character = (unsigned char) source.text[i];
        if (isspace(character)) {
            inWord = false;
        } else if (character && !inWord) {
            result.count++;
            inWord = true;
        }
    }

    result.state.ok = true;
    result.state.errorCode = RCN_ERR_NONE;

    return result;
}
