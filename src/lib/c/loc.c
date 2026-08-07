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

static void skipSpaceCharacters(Span* segment) {
    assert(segment != NULL);
    while (segment->length > 0 && isAsciiSpace(*segment->ptr)) {
        segment->ptr += 1;
        segment->length -= 1;
    }
}

static const char* searchBlockClosingMarker(Span span, Span blockEnd) {
    const char* closingFound = NULL;
    if (blockEnd.length > 0 && span.length >= blockEnd.length) {
        const size_t maxOffset = span.length - blockEnd.length;
        for (size_t offset = 0; offset <= maxOffset; ++offset) {
            const char* const position = span.ptr + offset;
            if (memcmp(position, blockEnd.ptr, blockEnd.length) == 0) {
                closingFound = position;
                break;
            }
        }
    }
    return closingFound;
}

static bool hasCommentMarker(Span segment, Span commentType) {
    return commentType.length > 0
        && segment.length >= commentType.length
        && memcmp(segment.ptr, commentType.ptr, commentType.length) == 0;
}

/**
 * Scans the span to determine whether it contains actual source code,
 * updating the block-comment tracking state as boundaries are crossed.
 *
 * A line segment is counted as source code if it contains at least one
 * character that is not whitespace and not part of a comment.
 *
 * @param segment A span that indicates the start of the line segment
 *                (inclusive) and the length of the line segment (exclusive).
 *                The span's ptr + length should point at the NL byte or one
 *                past the last byte of text.
 * @param lineComment Line-comment start marker span.
 * @param blockStart Block-comment start marker span.
 * @param blockEnd Block-comment end marker span.
 * @param inBlockComment Out param: whether currently inside a block
 *                       comment when entering the segment.
 * @return True if the line segment contains source code.
 */
static bool lineHasCode(
    Span segment,
    Span lineComment,
    Span blockStart,
    Span blockEnd,
    bool* inBlockComment
) {
    const char* const segmentEnd = segment.ptr + segment.length;
    while (segment.ptr < segmentEnd) {
        if (*inBlockComment) {
            const char* found = searchBlockClosingMarker(segment, blockEnd);
            if (found) {
                segment.ptr = found + blockEnd.length;
                segment.length = segmentEnd - segment.ptr;
                *inBlockComment = false;
                continue;
            }
            return false; // Entire segment is within a block comment
        }

        skipSpaceCharacters(&segment);

        if (segment.length == 0) {
            return false; // Has only whitespace, no code
        }

        if (hasCommentMarker(segment, lineComment)) {
            return false;
        }

        if (hasCommentMarker(segment, blockStart)) {
            // Search for the matching closing marker on the same line
            const char* const searchStart = segment.ptr + blockStart.length;
            const char* const closingFound = searchBlockClosingMarker(
                (Span){searchStart, segmentEnd - searchStart},
                blockEnd
            );
            if (!closingFound) {
                *inBlockComment = true;
                return false; // Block comment continues on the next line
            }
            // Is inline block comment. Skip over it and continue scanning
            segment.ptr = closingFound + blockEnd.length;
            segment.length = segmentEnd - segment.ptr;
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
    Span lineComment,
    Span blockStart,
    Span blockEnd
) {
    RcnCount count = 0;

    // Skip UTF-8 BOM if present
    if (hasUTF8BOM(source)) {
        source.text += 3;
        source.size -= 3;
    }

    bool inBlockComment = false;
    const char* position = source.text;
    const char* const textEnd = source.text + source.size;

    while (position < textEnd) {
        // Locate the end of the current line
        const char* lineEnd = position;
        while (lineEnd < textEnd && *lineEnd != '\n') {
            ++lineEnd;
        }

        const bool lineHasSourceCode = lineHasCode(
            (Span){position, lineEnd - position},
            lineComment,
            blockStart,
            blockEnd,
            &inBlockComment
        );
        if (lineHasSourceCode) {
            ++count;
        }

        // Advance past the NL, or stop if at end of text
        position = (lineEnd < textEnd) ? lineEnd + 1 : textEnd;
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
 * as UTF-16 code units within the byte range of span `text`.
 * The search advances in 2-byte steps.
 *
 * Returns the byte offset of the first match, or `SIZE_MAX` if not found.
 */
static size_t utf16FindAscii(Span text, Span target, bool isLittleEndian) {
    if (target.length > SIZE_MAX / 2) {
        return SIZE_MAX; // LCOV_EXCL_LINE
    }
    const size_t targetBytes = target.length * 2;
    if (targetBytes == 0 || targetBytes > text.length) {
        return SIZE_MAX;
    }
    for (size_t i = 0; i + targetBytes <= text.length; i += 2) {
        bool match = true;
        for (size_t j = 0; j < target.length; ++j) {
            unsigned char character = utf16AsciiAt(
                text.ptr,
                i + j * 2,
                isLittleEndian
            );
            if (character != (unsigned char) target.ptr[j]) {
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

static bool utf16HasNewlineAt(Span text, size_t offset, bool isLittleEndian) {
    return (offset + 1) < text.length
        && utf16AsciiAt(text.ptr, offset, isLittleEndian) == '\n';
}

/**
 * Returns the byte offset of the current UTF-16 line ending (`\n`) or the
 * first byte past the end of available input if the line has no terminator.
 */
static size_t utf16LineEndOffset(
    Span text,
    size_t offset,
    bool isLittleEndian
) {
    size_t lineEndOffset = offset;
    while ((lineEndOffset + 1) < text.length) {
        if (utf16HasNewlineAt(text, lineEndOffset, isLittleEndian)) {
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
    Span blockEnd,
    size_t* scan,
    bool* inBlockComment
) {
    const size_t remaining = lineEndOffset - *scan;
    const size_t found = utf16FindAscii(
        (Span){text + *scan, remaining},
        blockEnd,
        isLittleEndian
    );
    if (found == SIZE_MAX) {
        return false;
    }
    *scan += found + (blockEnd.length * 2);
    *inBlockComment = false;
    return true;
}

static bool utf16StartsWithAsciiAt(
    const char* text,
    size_t scan,
    bool isLittleEndian,
    Span marker
) {
    return (
        marker.length > 0
        && utf16FindAscii(
            (Span){text + scan, marker.length * 2},
            marker,
            isLittleEndian
        ) == 0
    );
}

static bool utf16HasCommentMarker(
    Span segment,
    Span commentType,
    bool isLittleEndian
) {
    return commentType.length > 0
        && segment.length >= (commentType.length * 2)
        && utf16StartsWithAsciiAt(
            segment.ptr,
            0,
            isLittleEndian,
            commentType
        );
}

static bool utf16ConsumeInlineBlockComment(
    const char* text,
    size_t lineEndOffset,
    bool isLittleEndian,
    Span blockStart,
    Span blockEnd,
    size_t* scan,
    bool* inBlockComment
) {
    const size_t afterStart = *scan + (blockStart.length * 2);
    const size_t searchLen = lineEndOffset - afterStart;
    size_t closingFound = SIZE_MAX;

    if (blockEnd.length > 0) {
        closingFound = utf16FindAscii(
            (Span){text + afterStart, searchLen},
            blockEnd,
            isLittleEndian
        );
    }
    if (closingFound == SIZE_MAX) {
        *inBlockComment = true;
        return false;
    }

    *scan = afterStart + closingFound + (blockEnd.length * 2);
    return true;
}

static bool utf16LineHasCode(
    const char* text,
    size_t offset,
    size_t lineEndOffset,
    bool isLittleEndian,
    Span lineComment,
    Span blockStart,
    Span blockEnd,
    bool* inBlockComment
) {
    size_t position = offset;

    while ((position + 1) <= lineEndOffset) {
        if (*inBlockComment) {
            bool hasMultiLineBlockComment = !utf16AdvanceOverBlockComment(
                text,
                lineEndOffset,
                isLittleEndian,
                blockEnd,
                &position,
                inBlockComment
            );
            if (hasMultiLineBlockComment) {
                return false;
            }
            continue;
        }

        char character = (char) utf16AsciiAt(text, position, isLittleEndian);
        if (isAsciiSpace(character)) {
            position += 2;
            continue;
        }

        const size_t remaining = lineEndOffset - position;

        const bool hasLineComment = utf16HasCommentMarker(
            (Span){text + position, remaining},
            lineComment,
            isLittleEndian
        );
        if (hasLineComment) {
            return false;
        }

        const bool hasBlockComment = utf16HasCommentMarker(
            (Span){text + position, remaining},
            blockStart,
            isLittleEndian
        );
        if (hasBlockComment) {
            bool isMultiLineBlockComment = !utf16ConsumeInlineBlockComment(
                text,
                lineEndOffset,
                isLittleEndian,
                blockStart,
                blockEnd,
                &position,
                inBlockComment
            );
            if (isMultiLineBlockComment) {
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
 * `text.ptr` points to the first byte after the BOM.
 * `text.length` is the number of remaining bytes.
 */
static RcnCount countLocUTF16(
    Span text,
    bool isLittleEndian,
    Span lineComment,
    Span blockStart,
    Span blockEnd
) {
    RcnCount count = 0;
    bool inBlockComment = false;
    size_t offset = 0; // Current byte position (always even)

    while ((offset + 1) < text.length) {
        const size_t lineEndOffset = utf16LineEndOffset(
            text,
            offset,
            isLittleEndian
        );

        const bool lineHasSourceCode = utf16LineHasCode(
            text.ptr,
            offset,
            lineEndOffset,
            isLittleEndian,
            lineComment,
            blockStart,
            blockEnd,
            &inBlockComment
        );

        if (lineHasSourceCode) {
            ++count;
        }

        if (utf16HasNewlineAt(text, lineEndOffset, isLittleEndian)) {
            offset = lineEndOffset + 2;
        } else {
            offset = text.length;
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
            "The LOC count input format or programming language "
            "is not supported"
        );
        return result;
    }

    const Span lineComment = getInlineSourceCommentString(language);
    const Span blockStart = getBlockCommentStartString(language);
    const Span blockEnd = getBlockCommentEndString(language);

    TextEncoding encoding = detectEncoding(sourceCode);
    if (encoding == TextEncodingUTF8) {
        result.count = countLocUTF8(
            sourceCode,
            lineComment,
            blockStart,
            blockEnd
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
            (Span){text, size},
            isLittleEndian,
            lineComment,
            blockStart,
            blockEnd
        );
    }

    result.state.ok = true;
    result.state.errorCode = RCN_ERR_NONE;
    return result;
}
