/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


//@todo:suhas: make it work on Windows as well
#include <unistd.h>
#define GET_CURRENT_DIR getcwd

#include <sys/stat.h>  // To check for directory existence.
#ifndef S_ISDIR  // Not defined in stat.h on Windows.
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#include <cstdio>

#include "typedefs.h"  // For architecture defines


//@todo:suhas: make it work for windows as well
static const char* kPathDelimiter = "/";

// The file we're looking for to identify the project root dir.
static const char* kProjectRootFileName = "LEGAL";
static const char* kFallbackPath = "./";
const char* kCannotFindProjectRootDir = "ERROR_CANNOT_FIND_PROJECT_ROOT_DIR";

std::string WorkingDir() {
  char path_buffer[FILENAME_MAX];
  if (!GET_CURRENT_DIR(path_buffer, sizeof(path_buffer))) {
    fprintf(stderr, "Cannot get current directory!\n");
    return kFallbackPath;
  } else {
    return std::string(path_buffer);
  }
}


std::string ProjectRootPath() {
  std::string working_dir = WorkingDir();
  if (working_dir == kFallbackPath) {
    return kCannotFindProjectRootDir;
  }
  // Check for our file that verifies the root dir.
  std::string current_path(working_dir);
  FILE* file = NULL;
  int path_delimiter_index = current_path.find_last_of(kPathDelimiter);
  while (path_delimiter_index > -1) {
    std::string root_filename = current_path + kPathDelimiter +
        kProjectRootFileName;
    file = fopen(root_filename.c_str(), "r");
    if (file != NULL) {
      fclose(file);
      return current_path + kPathDelimiter;
    }
    // Move up one directory in the directory tree.
    current_path = current_path.substr(0, path_delimiter_index);
    path_delimiter_index = current_path.find_last_of(kPathDelimiter);
  }
  // Reached the root directory.
  fprintf(stderr, "Cannot find project root directory!\n");
  return kCannotFindProjectRootDir;
}


