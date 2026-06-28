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
#include <string.h>
#include <assert.h>

#include "reckon/reckon.h"
#include "evaluation.h"

/**
 * Returns true if the given byte is an ASCII whitespace character
 * (space, tab, carriage return, form feed, or vertical tab).
 */
static inline bool isAsciiSpace(char character) {
    return character == ' '
        || character == '\t'
        || character == '\r'
        || character == '\f'
        || character == '\v';
}

static const char* searchBlockClosingMarker(
    const char* ptr,
    const char* end,
    const char* lineComment,
    size_t lineComLen,
    const char* blockStart,
    size_t blockStartLen,
    const char* blockEnd,
    size_t blockEndLen
) {
    size_t searchLen = (size_t) (end - ptr);
    const char* closingFound = NULL;
    if (blockEndLen > 0 && searchLen >= blockEndLen) {
        const size_t maxOffset = searchLen - blockEndLen;
        for (size_t offset = 0; offset <= maxOffset; ++offset) {
            if (memcmp(ptr + offset, blockEnd, blockEndLen) == 0) {
                closingFound = ptr + offset;
                break;
            }
        }
    }
    return closingFound;
}

/**
 * Scans the byte range [ptr, end) to determine whether it contains actual
 * source code, updating the block-comment tracking state as boundaries
 * are crossed.
 *
 * A line segment is counted as source code if it contains at least one
 * character that is not whitespace and not part of a comment.
 *
 * @param ptr Start of the line segment (inclusive).
 * @param end End of the line segment (exclusive). Should point at the NL byte
 *            or one past the last byte of text.
 * @param lineComment Null-terminated line-comment start marker, or NULL.
 * @param lineComLen strlen(lineComment), or 0 if lineComment is NULL.
 * @param blockStart Null-terminated block-comment start marker, or NULL.
 * @param blockStartLen strlen(blockStart), or 0 if blockStart is NULL.
 * @param blockEnd Null-terminated block-comment end marker, or NULL.
 * @param blockEndLen strlen(blockEnd), or 0 if blockEnd is NULL.
 * @param inBlockComment Out param: whether currently inside a block
 *                       comment when entering the segment.
 * @return True if the line segment contains source code.
 */
static bool segmentHasCode(
    const char* ptr,
    const char* end,
    const char* lineComment,
    size_t lineComLen,
    const char* blockStart,
    size_t blockStartLen,
    const char* blockEnd,
    size_t blockEndLen,
    bool* inBlockComment
) {
    while (ptr < end) {
        if (*inBlockComment) {
            // Look for the end of the block comment in this segment
            const char* found = searchBlockClosingMarker(
                ptr, end,
                lineComment, lineComLen,
                blockStart, blockStartLen,
                blockEnd, blockEndLen
            );
            if (!found) {
                return false; // Entire segment is within a block comment
            }
            ptr = found + blockEndLen;
            *inBlockComment = false;
            continue;
        }

        if (isAsciiSpace(*ptr)) {
            ++ptr;
            continue;
        }

        const size_t remaining = (size_t) (end - ptr);

        // Check for line-comment marker
        if (lineComLen > 0
            && remaining >= lineComLen
            && memcmp(ptr, lineComment, lineComLen) == 0) {

            return false;
        }

        // Check for block-comment start marker
        if (blockStartLen > 0
            && remaining >= blockStartLen
            && memcmp(ptr, blockStart, blockStartLen) == 0) {

            // Search for the matching closing marker on the same line
            const char* closingFound = searchBlockClosingMarker(
                ptr + blockStartLen, end,
                lineComment, lineComLen,
                blockStart, blockStartLen,
                blockEnd, blockEndLen
            );
            if (!closingFound) {
                *inBlockComment = true;
                return false; // Block comment continues on the next line
            }
            // Is inline block comment. Skip over it and continue scanning
            ptr = closingFound + blockEndLen;
            continue;
        }

        // Non-whitespace, non-comment character, so this line has source code
        return true;
    }
    return false;
}

/**
 * Counts LOC in UTF-8 encoded text (with or without BOM).
 */
static RcnCount countLocUTF8(
    RcnSourceText source,
    const char* lineComment,
    size_t lineComLen,
    const char* blockStart,
    size_t blockStartLen,
    const char* blockEnd,
    size_t blockEndLen
) {
    RcnCount count = 0;
    const char* text = source.text;
    size_t size = source.size;

    // Skip UTF-8 BOM if present
    if (hasUTF8BOM(source)) {
        text += 3;
        size -= 3;
    }

    bool inBlockComment = false;
    const char* pos = text;
    const char* const textEnd = text + size;

    while (pos < textEnd) {
        // Locate the end of the current line
        const char* lineEnd = pos;
        while (lineEnd < textEnd && *lineEnd != '\n') {
            ++lineEnd;
        }

        const bool lineHasSourceCode = segmentHasCode(
            pos, lineEnd,
            lineComment, lineComLen,
            blockStart, blockStartLen,
            blockEnd, blockEndLen,
            &inBlockComment
        );
        if (lineHasSourceCode) {
            ++count;
        }

        // Advance past the NL, or stop if at end of text
        pos = (lineEnd < textEnd) ? lineEnd + 1 : textEnd;
    }

    return count;
}

/**
 * Returns the ASCII code of the UTF-16 code unit at byte offset `offset`
 * within `text`, or 0 if the code unit is not a pure ASCII character
 * or represents the null character.
 *
 * No bounds checking is performed. The caller must ensure at least two bytes
 * are available at `offset`.
 */
static inline unsigned char utf16AsciiAt(
    const char* text,
    size_t offset,
    bool isLittleEndian
) {
    unsigned char low = (unsigned char)(
        isLittleEndian ? text[offset] : text[offset + 1]
    );
    unsigned char high = (unsigned char)(
        isLittleEndian ? text[offset + 1] : text[offset]
    );
    return (high == 0 && low > 0 && low < 128) ? low : 0;
}

/**
 * Searches for the first occurrence of the ASCII string `target` encoded
 * as UTF-16 code units within the byte range [text, text+length).
 * The search advances in 2-byte steps.
 *
 * Returns the byte offset of the first match, or `SIZE_MAX` if not found.
 */
static size_t utf16FindAscii(
    const char* text,
    size_t length,
    const char* target,
    size_t targetLength,
    bool isLittleEndian
) {
    const size_t targetBytes = targetLength * 2;
    if (targetBytes == 0 || targetBytes > length) {
        return SIZE_MAX;
    }
    for (size_t i = 0; i + targetBytes <= length; i += 2) {
        bool match = true;
        for (size_t j = 0; j < targetLength; ++j) {
            unsigned char character = utf16AsciiAt(
                text,
                i + j * 2,
                isLittleEndian
            );
            if (character != (unsigned char) target[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return SIZE_MAX;
}

/**
 * Returns the byte offset of the current UTF-16 line ending (`\n`) or the
 * first byte past the end of available input if the line has no terminator.
 */
static size_t utf16LineEndOffset(
    const char* text,
    size_t size,
    size_t offset,
    bool isLittleEndian
) {
    size_t lineEndOffset = offset;
    while ((lineEndOffset + 1) < size) {
        unsigned char character = utf16AsciiAt(
            text,
            lineEndOffset,
            isLittleEndian
        );
        if (character == '\n') {
            break;
        }
        lineEndOffset += 2;
    }
    return lineEndOffset;
}

static bool utf16AdvanceOverBlockComment(
    const char* text,
    size_t lineEndOffset,
    bool isLittleEndian,
    const char* blockEnd,
    size_t blockEndLen,
    size_t* scan,
    bool* inBlockComment
) {
    const size_t remaining = lineEndOffset - *scan;
    const size_t found = utf16FindAscii(
        text + *scan,
        remaining,
        blockEnd, blockEndLen,
        isLittleEndian
    );
    if (found == SIZE_MAX) {
        return false;
    }
    *scan += found + (blockEndLen * 2);
    *inBlockComment = false;
    return true;
}

static bool utf16StartsWithAsciiAt(
    const char* text,
    size_t scan,
    bool isLittleEndian,
    const char* marker,
    size_t markerLen
) {
    return (
        markerLen > 0
        && utf16FindAscii(
            text + scan,
            markerLen * 2,
            marker,
            markerLen,
            isLittleEndian
        ) == 0
    );
}

static bool utf16ConsumeInlineBlockComment(
    const char* text,
    size_t lineEndOffset,
    bool isLittleEndian,
    const char* blockEnd,
    size_t blockEndLen,
    size_t* scan,
    bool* inBlockComment,
    size_t blockStartLen
) {
    const size_t afterStart = *scan + (blockStartLen * 2);
    const size_t searchLen = lineEndOffset - afterStart;
    size_t closingFound = SIZE_MAX;

    if (blockEndLen > 0) {
        closingFound = utf16FindAscii(
            text + afterStart,
            searchLen,
            blockEnd,
            blockEndLen,
            isLittleEndian
        );
    }
    if (closingFound == SIZE_MAX) {
        *inBlockComment = true;
        return false;
    }

    *scan = afterStart + closingFound + (blockEndLen * 2);
    return true;
}

static bool utf16LineHasCode(
    const char* text,
    bool isLittleEndian,
    const char* lineComment,
    size_t lineComLen,
    const char* blockStart,
    size_t blockStartLen,
    const char* blockEnd,
    size_t blockEndLen,
    size_t offset,
    size_t lineEndOffset,
    bool* inBlockComment
) {
    size_t scan = offset;

    while ((scan + 1) <= lineEndOffset) {
        if (*inBlockComment) {
            if (!utf16AdvanceOverBlockComment(
                text,
                lineEndOffset,
                isLittleEndian,
                blockEnd,
                blockEndLen,
                &scan,
                inBlockComment)
            ) {
                return false;
            }
            continue;
        }

        char character = (char) utf16AsciiAt(text, scan, isLittleEndian);
        if (isAsciiSpace(character)) {
            scan += 2;
            continue;
        }

        const size_t remaining = lineEndOffset - scan;

        if (lineComLen > 0 && remaining >= (lineComLen * 2)
            && utf16StartsWithAsciiAt(
                text,
                scan,
                isLittleEndian,
                lineComment,
                lineComLen
            )
        ) {
            return false;
        }

        if (blockStartLen > 0 && remaining >= (blockStartLen * 2)
            && utf16StartsWithAsciiAt(
                text,
                scan,
                isLittleEndian,
                blockStart,
                blockStartLen)) {

            if (!utf16ConsumeInlineBlockComment(
                text,
                lineEndOffset,
                isLittleEndian,
                blockEnd,
                blockEndLen,
                &scan,
                inBlockComment,
                blockStartLen
            )) {
                return false;
            }
            continue;
        }

        return true;
    }

    return false;
}

/**
 * Counts LOC in UTF-16 encoded text (LE or BE).
 * `text` points to the first byte after the BOM.
 * `size` is the number of remaining bytes.
 */
static RcnCount countLocUTF16(
    const char* text,
    size_t size,
    bool isLittleEndian,
    const char* lineComment,
    size_t lineComLen,
    const char* blockStart,
    size_t blockStartLen,
    const char* blockEnd,
    size_t blockEndLen
) {
    RcnCount count = 0;
    bool inBlockComment = false;
    size_t offset = 0; // Current byte position (always even)

    while (offset + 1 < size) {
        const size_t lineEndOffset = utf16LineEndOffset(
            text,
            size,
            offset,
            isLittleEndian
        );

        const bool lineHasSourceCode = utf16LineHasCode(
            text,
            isLittleEndian,
            lineComment,
            lineComLen,
            blockStart,
            blockStartLen,
            blockEnd,
            blockEndLen,
            offset,
            lineEndOffset,
            &inBlockComment
        );

        if (lineHasSourceCode) {
            ++count;
        }

        if ((lineEndOffset + 1) < size
            && utf16AsciiAt(text, lineEndOffset, isLittleEndian) == '\n') {
            offset = lineEndOffset + 2;
        } else {
            offset = size;
        }
    }

    return count;
}

RcnCountResult rcnCountLinesOfCode(
    RcnTextFormat language,
    RcnSourceText sourceCode
) {
    RcnCountResult result = {0};

    if (sourceCode.size == 0) {
        result.state.ok = true;
        result.state.errorCode = RCN_ERR_NONE;
        return result;
    }
    if (!sourceCode.text) {
        result.state.errorCode = RCN_ERR_INVALID_INPUT;
        result.state.errorMessage = "Source code input must not be NULL";
        return result;
    }
    if (sourceCode.size > UINT32_MAX) {
        result.state.errorCode = RCN_ERR_INPUT_TOO_LARGE;
        result.state.errorMessage = "Input exceeds maximum supported size";
        return result;
    }

    if (!isLocEnabled(language)) {
        result.state.errorCode = RCN_ERR_UNSUPPORTED_FORMAT;
        result.state.errorMessage = (
            "The input format or programming language is not supported"
        );
        return result;
    }

    const char* lineComment = getInlineSourceCommentString(language);
    const char* blockStart = getBlockCommentStartString(language);
    const char* blockEnd = getBlockCommentEndString(language);

    const size_t lineComLength = lineComment ? strlen(lineComment) : 0;
    const size_t blockStartLength = blockStart ? strlen(blockStart) : 0;
    const size_t blockEndLength = blockEnd ? strlen(blockEnd) : 0;

    TextEncoding encoding = detectEncoding(sourceCode);
    if (encoding == TextEncodingUTF8) {
        result.count = countLocUTF8(
            sourceCode,
            lineComment, lineComLength,
            blockStart, blockStartLength,
            blockEnd, blockEndLength
        );
    } else {
        assert(
            encoding == TextEncodingUTF16LE || encoding == TextEncodingUTF16BE
        );
        const bool isLittleEndian = (encoding == TextEncodingUTF16LE);
        // The BOM occupies the first 2 bytes
        const char* text = sourceCode.text + 2;
        const size_t size = sourceCode.size >= 2 ? sourceCode.size - 2 : 0;
        result.count = countLocUTF16(
            text, size,
            isLittleEndian,
            lineComment, lineComLength,
            blockStart, blockStartLength,
            blockEnd, blockEndLength
        );
    }

    result.state.ok = true;
    result.state.errorCode = RCN_ERR_NONE;
    return result;
}
