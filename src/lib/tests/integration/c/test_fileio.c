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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"

#include "reckon/reckon.h"
#include "fileio.h"

#define FILE_SAMPLE_EXT "txt"
#define PATH_DIR_RES1 RECKON_TEST_PATH_RES_BASE "/txt"
#define PATH_DIR_RES2 PATH_DIR_RES1 "/res2"
#define PATH_DIR_RES3 PATH_DIR_RES2 "/res3"
#define PATH_DIR_RES4 PATH_DIR_RES3 "/res4"
#define FILE_SAMPLE1 "sample1.txt"
#define FILE_SAMPLE2 "sample2.txt"
#define FILE_SAMPLE3 "sample3.txt"
#define PATH_SAMPLE_DIR1_FILE1 PATH_DIR_RES1 "/1" FILE_SAMPLE1
#define PATH_SAMPLE_DIR1_FILE2 PATH_DIR_RES1 "/1" FILE_SAMPLE2
#define PATH_SAMPLE_DIR1_FILE3 PATH_DIR_RES1 "/1" FILE_SAMPLE3
#define PATH_SAMPLE_DIR2_FILE1 PATH_DIR_RES2 "/2" FILE_SAMPLE1
#define PATH_SAMPLE_DIR2_FILE2 PATH_DIR_RES2 "/2" FILE_SAMPLE2
#define PATH_SAMPLE_DIR3_FILE1 PATH_DIR_RES3 "/3" FILE_SAMPLE1
#define PATH_SAMPLE_JAVA_FILE RECKON_TEST_PATH_RES_BASE "/java/Sample.java"

void setUp(void) { }

void tearDown(void) { }

// NOLINTBEGIN(readability-magic-numbers)

static void assertValidFileListItem(
    RcnSourceFile* file,
    const char* expectedName,
    const char* expectedPath
) {
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(0, file->content.size);
    TEST_ASSERT_EQUAL_STRING(expectedName, file->name);
    TEST_ASSERT_EQUAL_STRING(FILE_SAMPLE_EXT, file->extension);
    TEST_ASSERT_EQUAL_STRING(expectedPath, file->path);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file->status);
}

void testIsDirectoryWithDirectory(void) {
    char* dirPath = RECKON_TEST_PATH_RES_BASE;
    bool isDir = isDirectory(dirPath);
    TEST_ASSERT_TRUE(isDir);
}

void testIsDirectoryWithRegularFile(void) {
    char* filePath = PATH_SAMPLE_DIR1_FILE1;
    bool isDir = isDirectory(filePath);
    TEST_ASSERT_FALSE(isDir);
}

void testIsDirectoryWithNonExistentFile(void) {
    char* nonexistent = RECKON_TEST_PATH_RES_BASE "/this-file-does-not-exist";
    bool isDir = isDirectory(nonexistent);
    TEST_ASSERT_FALSE(isDir);
}

void testIsValidStatsInputWorksOnRegularFiles(void) {
    char* path = PATH_SAMPLE_DIR1_FILE1;
    const char* errorMessage = isValidStatsInput(path);
    TEST_ASSERT_NULL(errorMessage);
}

void testIsValidStatsInputWorksOnDirectories(void) {
    char* path = RECKON_TEST_PATH_RES_BASE;
    const char* errorMessage = isValidStatsInput(path);
    TEST_ASSERT_NULL(errorMessage);
}

void testIsValidStatsInputWorksOnNonExistentFiles(void) {
    char* path = RECKON_TEST_PATH_RES_BASE "/this-file-does-not-exist";
    const char* errorMessage = isValidStatsInput(path);
    TEST_ASSERT_NOT_NULL(errorMessage);
    TEST_ASSERT_EQUAL_STRING("No such file or directory", errorMessage);
}

void testCreateSourceFileWithNullInput(void) {
    RcnSourceFile* file = newSourceFile(NULL);
    TEST_ASSERT_NULL(file);
}

void testCreateSourceFile(void) {
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_DIR1_FILE1);
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file->content.size);
    TEST_ASSERT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_STRING("1" FILE_SAMPLE1, file->name);
    TEST_ASSERT_EQUAL_STRING("txt", file->extension);
    TEST_ASSERT_EQUAL_STRING(PATH_SAMPLE_DIR1_FILE1, file->path);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file->status);
    freeSourceFile(file);
}

void testInitSourceFile(void) {
    char* path = PATH_SAMPLE_DIR1_FILE1;
    RcnSourceFile file = {0};
    initSourceFile(&file, path);
    TEST_ASSERT_FALSE(file.isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file.content.size);
    TEST_ASSERT_NULL(file.content.text);
    TEST_ASSERT_EQUAL_STRING("1" FILE_SAMPLE1, file.name);
    TEST_ASSERT_EQUAL_STRING("txt", file.extension);
    TEST_ASSERT_EQUAL_STRING(path, file.path);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file.status);
    deinitSourceFile(&file);
}

void testDeinitSourceFileFreesContent(void) {
    RcnSourceFile file = {0};
    initSourceFile(&file, PATH_SAMPLE_DIR1_FILE1);
    size_t contentSize = 96;
    file.content.text = malloc(contentSize);
    memset(file.content.text, 'A', contentSize);
    file.isContentRead = true;
    file.content.size = contentSize;
    deinitSourceFile(&file);
    TEST_ASSERT_FALSE(file.isContentRead);
    TEST_ASSERT_EQUAL_INT(0, file.content.size);
    TEST_ASSERT_NULL(file.content.text);
}

void testReadSourceFile(void) {
    size_t expectedSize = 26;
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_DIR1_FILE1);
    bool success = readSourceFileContent(file);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_TRUE(file->isContentRead);
    TEST_ASSERT_NOT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(expectedSize, file->content.size);
    TEST_ASSERT_EQUAL_STRING("File: res/txt/1sample1.txt", file->content.text);
    // Read should be idempotent
    success = readSourceFileContent(file);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_TRUE(file->isContentRead);
    TEST_ASSERT_NOT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(expectedSize, file->content.size);
    freeSourceFile(file);
}

void testFreeSourceFileContent(void) {
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_DIR1_FILE1);
    bool success = readSourceFileContent(file);
    TEST_ASSERT_TRUE(success);
    freeSourceFileContent(file);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(0, file->content.size);
    freeSourceFile(file);
}

void testReadSourceFileContentOfNonExistentFileFails(void) {
    char* nonexistent = RECKON_TEST_PATH_RES_BASE "/this-file-does-not-exist";
    RcnSourceFile* file = newSourceFile(nonexistent);
    TEST_ASSERT_NOT_NULL(file);
    bool ok = readSourceFileContent(file);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_FILE_NOT_FOUND, file->status);
    freeSourceFileContent(file); // Should be safe to call
    freeSourceFile(file);
}

void testReadSourceFileContentWithFailedStateFails(void) {
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_DIR1_FILE1);
    file->status = RCN_FILE_OP_IO_ERROR;
    bool ok = readSourceFileContent(file);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_NULL(file->content.text);
    freeSourceFile(file);
}

void testReadSourceFileContentWithNullInputFails(void) {
    bool ok = readSourceFileContent(NULL);
    TEST_ASSERT_FALSE(ok);
}

void testReadSourceFileContentIsNotReadIfAlreadyRead(void) {
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_DIR1_FILE1);
    file->isContentRead = true;
    bool success = readSourceFileContent(file);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_TRUE(file->isContentRead);
    TEST_ASSERT_NULL(file->content.text); // Should remain NULL
    TEST_ASSERT_EQUAL_INT(0, file->content.size);
    freeSourceFile(file);
}

void testReadSourceFileFailsWhenFilePathIsNull(void) {
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_DIR1_FILE1);
    free(file->path);
    file->path = NULL;
    bool success = readSourceFileContent(file);
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_INVALID_PATH, file->status);
    TEST_ASSERT_FALSE(file->isContentRead);
    TEST_ASSERT_NULL(file->content.text);
    TEST_ASSERT_EQUAL_INT(0, file->content.size);
    freeSourceFile(file);
}

void testDetectSourceFormatSupported(void) {
    RcnSourceFile* file = newSourceFile(PATH_SAMPLE_JAVA_FILE);
    TEST_ASSERT_NOT_NULL(file);
    SourceFormatDetection detection = detectSourceFormat(file);
    TEST_ASSERT_TRUE(detection.isSupportedFormat);
    TEST_ASSERT_EQUAL_INT(RCN_LANG_JAVA, detection.format);
    freeSourceFile(file);
}

void testDetectSourceFormatUnsupported(void) {
    RcnSourceFile* file = newSourceFile("/tmp/res/unknown.xyz");
    TEST_ASSERT_NOT_NULL(file);
    SourceFormatDetection detection = detectSourceFormat(file);
    TEST_ASSERT_FALSE(detection.isSupportedFormat);
    freeSourceFile(file);
}

void testCreateSourceFileListOfEmptyDirectory(void) {
    char* emptyDirectory = PATH_DIR_RES4;
    SourceFileList fileList = newSourceFileList(emptyDirectory);
    TEST_ASSERT_TRUE(fileList.ok);
    TEST_ASSERT_EQUAL_INT(0, fileList.size);
    TEST_ASSERT_NULL(fileList.files);
}

void testCreateSourceFileListFailsWhenInputPathIsNull(void) {
    SourceFileList fileList = newSourceFileList(NULL);
    TEST_ASSERT_FALSE(fileList.ok);
    TEST_ASSERT_EQUAL_INT(0, fileList.size);
    TEST_ASSERT_NULL(fileList.files);
}

void testCreateSourceFileListOfDirectoryContainingOnlyOneValidFile(void) {
    char* dirPath = PATH_DIR_RES3;
    char* sampleFileName = "3" FILE_SAMPLE1;
    SourceFileList fileList = newSourceFileList(dirPath);
    TEST_ASSERT_TRUE(fileList.ok);
    TEST_ASSERT_EQUAL_INT(1, fileList.size);
    TEST_ASSERT_NOT_NULL(fileList.files);
    RcnSourceFile* file1 = &fileList.files[0];
    TEST_ASSERT_NOT_NULL(file1);
    TEST_ASSERT_FALSE(file1->isContentRead);
    TEST_ASSERT_NULL(file1->content.text);
    TEST_ASSERT_EQUAL_INT(0, file1->content.size);
    TEST_ASSERT_EQUAL_STRING(sampleFileName, file1->name);
    TEST_ASSERT_EQUAL_STRING("txt", file1->extension);
    TEST_ASSERT_EQUAL_STRING(PATH_SAMPLE_DIR3_FILE1, file1->path);
    TEST_ASSERT_EQUAL_INT(RCN_FILE_OP_OK, file1->status);
    freeSourceFileList(&fileList);
}

void testCreateSourceFileListOfDirectoryWithMoreSubdirectories(void) {
    char* dirPath = PATH_DIR_RES1;
    SourceFileList fileList = newSourceFileList(dirPath);
    TEST_ASSERT_TRUE(fileList.ok);
    TEST_ASSERT_EQUAL_INT(6, fileList.size);
    TEST_ASSERT_NOT_NULL(fileList.files);
    RcnSourceFile* file1 = &fileList.files[0];
    assertValidFileListItem(file1, "1" FILE_SAMPLE1, PATH_SAMPLE_DIR1_FILE1);
    RcnSourceFile* file2 = &fileList.files[1];
    assertValidFileListItem(file2, "1" FILE_SAMPLE2, PATH_SAMPLE_DIR1_FILE2);
    RcnSourceFile* file3 = &fileList.files[2];
    assertValidFileListItem(file3, "1" FILE_SAMPLE3, PATH_SAMPLE_DIR1_FILE3);
    RcnSourceFile* file4 = &fileList.files[3];
    assertValidFileListItem(file4, "2" FILE_SAMPLE1, PATH_SAMPLE_DIR2_FILE1);
    RcnSourceFile* file5 = &fileList.files[4];
    assertValidFileListItem(file5, "2" FILE_SAMPLE2, PATH_SAMPLE_DIR2_FILE2);
    RcnSourceFile* file6 = &fileList.files[5];
    assertValidFileListItem(file6, "3" FILE_SAMPLE1, PATH_SAMPLE_DIR3_FILE1);
    freeSourceFileList(&fileList);
}

void testCreateSourceFileListOfDirectoryWithTrailingSlashInPath(void) {
    char* dirPath = PATH_DIR_RES1 "/";
    SourceFileList fileList = newSourceFileList(dirPath);
    TEST_ASSERT_TRUE(fileList.ok);
    TEST_ASSERT_EQUAL_INT(6, fileList.size);
    TEST_ASSERT_NOT_NULL(fileList.files);
    RcnSourceFile* file1 = &fileList.files[0];
    assertValidFileListItem(file1, "1" FILE_SAMPLE1, PATH_SAMPLE_DIR1_FILE1);
    RcnSourceFile* file2 = &fileList.files[1];
    assertValidFileListItem(file2, "1" FILE_SAMPLE2, PATH_SAMPLE_DIR1_FILE2);
    RcnSourceFile* file3 = &fileList.files[2];
    assertValidFileListItem(file3, "1" FILE_SAMPLE3, PATH_SAMPLE_DIR1_FILE3);
    RcnSourceFile* file4 = &fileList.files[3];
    assertValidFileListItem(file4, "2" FILE_SAMPLE1, PATH_SAMPLE_DIR2_FILE1);
    RcnSourceFile* file5 = &fileList.files[4];
    assertValidFileListItem(file5, "2" FILE_SAMPLE2, PATH_SAMPLE_DIR2_FILE2);
    RcnSourceFile* file6 = &fileList.files[5];
    assertValidFileListItem(file6, "3" FILE_SAMPLE1, PATH_SAMPLE_DIR3_FILE1);
    freeSourceFileList(&fileList);
}

// NOLINTEND(readability-magic-numbers)

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testIsDirectoryWithDirectory);
    RUN_TEST(testIsDirectoryWithRegularFile);
    RUN_TEST(testIsDirectoryWithNonExistentFile);
    RUN_TEST(testIsValidStatsInputWorksOnRegularFiles);
    RUN_TEST(testIsValidStatsInputWorksOnDirectories);
    RUN_TEST(testIsValidStatsInputWorksOnNonExistentFiles);
    RUN_TEST(testCreateSourceFileWithNullInput);
    RUN_TEST(testCreateSourceFile);
    RUN_TEST(testInitSourceFile);
    RUN_TEST(testDeinitSourceFileFreesContent);
    RUN_TEST(testReadSourceFile);
    RUN_TEST(testFreeSourceFileContent);
    RUN_TEST(testReadSourceFileContentOfNonExistentFileFails);
    RUN_TEST(testReadSourceFileContentWithFailedStateFails);
    RUN_TEST(testReadSourceFileContentWithNullInputFails);
    RUN_TEST(testReadSourceFileContentIsNotReadIfAlreadyRead);
    RUN_TEST(testReadSourceFileFailsWhenFilePathIsNull);
    RUN_TEST(testDetectSourceFormatSupported);
    RUN_TEST(testDetectSourceFormatUnsupported);
    RUN_TEST(testCreateSourceFileListOfEmptyDirectory);
    RUN_TEST(testCreateSourceFileListFailsWhenInputPathIsNull);
    RUN_TEST(testCreateSourceFileListOfDirectoryContainingOnlyOneValidFile);
    RUN_TEST(testCreateSourceFileListOfDirectoryWithMoreSubdirectories);
    RUN_TEST(testCreateSourceFileListOfDirectoryWithTrailingSlashInPath);
    return UNITY_END();
}
