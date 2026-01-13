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
#include <stdint.h>

#include "reckon/reckon.h"
#include "evaluation.h"

static const uint16_t UTF16_BOM_LE = 0xfffe;
static const uint16_t UTF16_BOM_BE = 0xfeff;
static const uint16_t HIGH_SURROGATE_START = 0xd800;
static const uint16_t HIGH_SURROGATE_END = 0xdbff;
static const uint16_t LOW_SURROGATE_START = 0xdc00;
static const uint16_t LOW_SURROGATE_END = 0xdfff;
static const unsigned char MASK_B2 = 0xe0;
static const unsigned char MASK_B3 = 0xf0;
static const unsigned char MASK_B4 = 0xf8;
static const unsigned char TWO_BYTE_SEQ = 0xc0;
static const unsigned char THREE_BYTE_SEQ = 0xe0;
static const unsigned char FOUR_BYTE_SEQ = 0xf0;

/**
 * Reads a 16-bit code unit from text at a given byte offset.
 * No range checks are performed.
 */
static uint16_t codeUnit(const char* text, size_t offset, bool littleEndian) {
    const unsigned char byte0 = (unsigned char) text[offset];
    const unsigned char byte1 = (unsigned char) text[offset + 1];
    return (
        littleEndian
        ? (uint16_t) (byte0 | (byte1 << 8))
        : (uint16_t) ((byte0 << 8) | byte1)
    );
}

static RcnCount countCharactersUTF16(RcnSourceText source, bool littleEndian) {
    RcnCount count = 0;
    const size_t size = source.size;
    size_t offset = 0;
    // Skip UTF-16 BOMs if present
    if (size >= 2) {
        uint16_t cu0 = codeUnit(source.text, 0, littleEndian);
        if (cu0 == UTF16_BOM_BE || cu0 == UTF16_BOM_LE) {
            offset = 2;
        }
    }
    while ((offset + 1) < size) {
        uint16_t cu0 = codeUnit(source.text, offset, littleEndian);
        offset += 2;
        if (cu0 == UTF16_BOM_BE || cu0 == UTF16_BOM_LE) {
            continue; // Ignore stray BOMs beyond start
        }
        if (cu0 >= HIGH_SURROGATE_START && cu0 <= HIGH_SURROGATE_END) {
            // High surrogate: check for a following low surrogate
            if ((offset + 1) < size) {
                uint16_t cu1 = codeUnit(source.text, offset, littleEndian);
                if (cu1 >= LOW_SURROGATE_START && cu1 <= LOW_SURROGATE_END) {
                    // Valid surrogate pair forms a single code point
                    offset += 2;
                    ++count;
                }
            }
            continue;
        }
        if (cu0 >= LOW_SURROGATE_START && cu0 <= LOW_SURROGATE_END) {
            continue; // Ignore stray low surrogates
        }
        // BMP code point
        ++count;
    }
    // Any trailing single byte is ignored
    return count;
}

static RcnCount countCharactersUTF8(RcnSourceText source) {
    RcnCount count = 0;
    size_t offset = 0;
    if (hasUTF8BOM(source)) {
        offset = 3; // Skip BOM
    }
    const size_t size = source.size;
    while (offset < size) {
        const unsigned char byte = (unsigned char) source.text[offset];
        // Always consume at least 1 byte to avoid stalling, e.g. in case
        // of encoding errors like truncated or invalid leading byte etc.
        // We do not validate continuation bytes. Invalid or truncated UTF-8
        // sequences are counted as single characters.
        size_t stride = 1;
        if ((byte & MASK_B2) == TWO_BYTE_SEQ && (offset + 1) < size) {
            stride = 2;
        } else if ((byte & MASK_B3) == THREE_BYTE_SEQ && (offset + 2) < size) {
            stride = 3;
        } else if ((byte & MASK_B4) == FOUR_BYTE_SEQ && (offset + 3) < size) {
            stride = 4;
        }
        offset += stride;
        ++count;
    }
    return count;
}

RcnCountResult rcnCountCharacters(RcnSourceText source) {
    RcnCountResult result = {
        .count = 0,
        .state.ok = true,
        .state.errorMessage = NULL,
        .state.errorCode = RCN_ERR_NONE
    };
    if (source.size == 0) {
        return result;
    }
    if (!source.text) {
        result.state.ok = false;
        result.state.errorCode = RCN_ERR_INVALID_INPUT;
        result.state.errorMessage = "Text input must not be NULL";
        return result;
    }
    if (source.size > UINT32_MAX) {
        result.state.ok = false;
        result.state.errorCode = RCN_ERR_INPUT_TOO_LARGE;
        result.state.errorMessage = "Input exceeds maximum supported size";
        return result;
    }

    TextEncoding encoding = detectEncoding(source);
    if (encoding == TextEncodingUTF8) {
        result.count = countCharactersUTF8(source);
    } else { // UTF-16 with BOM indicating endianness
        const bool isLittleEndian = (encoding == TextEncodingUTF16LE);
        result.count = countCharactersUTF16(source, isLittleEndian);
    }

    return result;
}
