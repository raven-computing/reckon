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

#include "reckon/reckon.h"
#include "evaluation.h"

RcnCountResult rcnCountPhysicalLines(RcnSourceText source) {
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

    TextEncoding encoding = detectEncoding(source);
    const char* content  = source.text;

    if (encoding == TextEncodingUTF8) {
        for (size_t i = 0; i < source.size; ++i) {
            if (content[i] == '\n') {
                result.count++;
            }
        }
        // Account for last line if not ending with newline
        if (content[source.size - 1] != '\n') {
            result.count++;
        }
    } else { // UTF-16
        const bool isLittleEndian = (encoding == TextEncodingUTF16LE);
        const char nlByte0 = isLittleEndian ? 0x0a : 0x00;
        const char nlByte1 = isLittleEndian ? 0x00 : 0x0a;
        for (size_t i = 2; i < source.size - 1; i+=2) {
            if (content[i] == nlByte0 && content[i + 1] == nlByte1) {
                result.count++;
            }
        }
        // Account for last line if not ending with newline
        if ((source.size > 2)
            && (content[source.size - 2] != nlByte0
                || content[source.size - 1] != nlByte1)) {

            result.count++;
        }
    }

    result.state.ok = true;
    result.state.errorCode = RCN_ERR_NONE;

    return result;
}
