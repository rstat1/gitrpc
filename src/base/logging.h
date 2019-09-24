/*
* Copyright (c) 2012 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef LOGGING
#define LOGGING

#include <base/common.h>

#if defined(OS_ANDROID)
#include <android/log.h>
#define TAG "XPAppFX-Native"
#define LOG(priority, tag, ...) __android_log_print(ANDROID_LOG_##priority, tag, __VA_ARGS__)
#endif

#if defined(_DEBUG)
#define CLOG(priority, ...) Log("Compositor", __VA_ARGS__)
#define _DEBUG 1
#define VERBOSE 2
#endif

#if _DEBUG
#define LOG_ARGS(message, ...) Log(__FILE__, __LINE__, message, __VA_ARGS__);
#define LOG_MSG(message) Log(__FILE__, __LINE__, message);
#define TRACE LOG_MSG(__FUNCTION__);
#else
#define LOG_ARGS(message, ...)
#define LOG_MSG(message)
#define TRACE
#endif
#define LOG_REL(message) Log(__FILE__, __LINE__, message);
#define LOG_REL_A(message, ...) Log(__FILE__, __LINE__, message, __VA_ARGS__);
BASEAPI void InitLog(const char* filename);
BASEAPI void writeToLog(const char* entry, bool deleteFile);
BASEAPI void writeToLog(const char* entry);
BASEAPI void Log(const char* tag, int line, const char format[], ...);
BASEAPI void LogShowOnlyTag(const char* tag);
BASEAPI void MonoLog(const char* entry, bool deleteFile);

class Logging {
public:
	static void InitLog(const char* filename);
	static void MonoLog(const char* entry, bool deleteFirst);
	static void writeToLog(const char* entry, bool deleteFile, bool autoAppendTag);
	static void writeToLog(const char* entry);
	static void Log(const char tag[], const char format[], ...);
	static void ShowOnlyTag(const char* tag);
	static const char* tagToShow;

private:
	static const char* filename;
};
#endif
