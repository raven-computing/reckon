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

#include <stdbool.h>
#include <stdint.h>

#include "reckon/reckon.h"
#include "evaluation.h"

static const uint8_t UTF8BOM_0 = 0xef;
static const uint8_t UTF8BOM_1 = 0xbb;
static const uint8_t UTF8BOM_2 = 0xbf;
static const uint8_t UTF16BOM_LE_0 = 0xff;
static const uint8_t UTF16BOM_LE_1 = 0xfe;
static const uint8_t UTF16BOM_BE_0 = 0xfe;
static const uint8_t UTF16BOM_BE_1 = 0xff;

bool hasUTF8BOM(RcnSourceText source) {
    if (source.size >= 3) {
        const uint8_t byte0 = (uint8_t) source.text[0];
        const uint8_t byte1 = (uint8_t) source.text[1];
        const uint8_t byte2 = (uint8_t) source.text[2];
        if (byte0 == UTF8BOM_0 && byte1 == UTF8BOM_1 && byte2 == UTF8BOM_2) {
            return true;
        }
    }
    return false;
}

TextEncoding detectEncoding(RcnSourceText source) {
    if (hasUTF8BOM(source)) {
        return TextEncodingUTF8;
    }
    if (source.size >= 2) {
        // Check for UTF-16 BOM
        const uint8_t byte0 = (uint8_t) source.text[0];
        const uint8_t byte1 = (uint8_t) source.text[1];
        if (byte0 == UTF16BOM_LE_0 && byte1 == UTF16BOM_LE_1) {
            return TextEncodingUTF16LE;
        }
        if (byte0 == UTF16BOM_BE_0 && byte1 == UTF16BOM_BE_1) {
            return TextEncodingUTF16BE;
        }
    }
    return TextEncodingUTF8; // Default
}
