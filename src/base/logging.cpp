/*
* Copyright (c) 2012 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/common.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

#if defined(OS_WIN)
#include <Windows.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>

#include <base/Utils.h>
#include <base/logging.h>

using namespace std;
using namespace base::utils;

const char *Logging::tagToShow = "All";
const char *Logging::filename;

void InitLog(const char *filename) {
	Logging::InitLog(filename);
}
void writeToLog(const char *entry, bool deleteFile) {
	Logging::writeToLog(entry, deleteFile, true);
}
void writeToLog(const char *entry) {
	Logging::writeToLog(entry, false, true);
}
void Log(const char *tag, int line, const char format[], ...) {
	va_list args;
	va_start(args, format);

#if defined(OS_ANDROID) || defined(MOBILEBOT)
	__android_log_vprint(ANDROID_LOG_DEBUG, tag, format, args);
#else
	static const size_t kBufferSize = 2048;
	char buffer[kBufferSize + 1];
	const char *TAG = tag;
	vsnprintf(buffer, kBufferSize, format, args);
	va_end(args);

	string logLine("");
	logLine.append(tag);
	logLine.append(":");
	logLine.append(to_string(line));
	logLine.append("] ");
	logLine.append(buffer);
	Logging::writeToLog(logLine.c_str(), false, false);

#endif
}
void LogShowOnlyTag(const char *tag) {
	Logging::ShowOnlyTag(tag);
}
void MonoLog(const char *entry, bool deleteFile) {
	Logging::MonoLog(entry, deleteFile);
}
void Logging::InitLog(const char *filename) {
	Logging::filename = filename;
	FILE *pFile = fopen(filename, "w");
	fclose(pFile);
}
void Logging::writeToLog(const char *entry, bool deleteFile, bool autoAppendTag) {
#if !defined(OS_ANDROID)
	FILE *pFile;

	string logEntry("");
	logEntry.append("[");
	logEntry.append(std::to_string(GetThreadID()));
#if defined(OS_LINUX)
	logEntry.append(";");
	// logEntry.append("");
	logEntry.append(program_invocation_short_name);
#endif
	logEntry.append(";");
	if (autoAppendTag) { logEntry.append("All] "); }
	logEntry.append(entry);

	if (deleteFile == true) {
		pFile = fopen(filename, "w");
	} else {
		pFile = fopen(filename, "a+");
	}

	if (pFile != NULL) {
		fputs(logEntry.c_str(), pFile);
		fputc('\n', pFile);

		fclose(pFile);
	}
#else
	LOG(INFO, TAG, entry);
#endif
}
void Logging::writeToLog(const char *entry) {
	writeToLog(entry, false, true);
}
void Logging::ShowOnlyTag(const char *tag) {
	Logging::tagToShow = tag;
}
void Logging::MonoLog(const char *entry, bool deleteFirst) {
#if _DEBUG
	FILE *pFile;

	string logEntry("");
	logEntry.append(entry);

	if (deleteFirst == true) {
		pFile = fopen("monologue.txt", "w");
	} else {
		pFile = fopen("monologue.txt", "a+");
	}

	if (pFile != NULL) {
		fputs(logEntry.c_str(), pFile);
		fputc('\n', pFile);

		fclose(pFile);
	}
#endif
}
void Logging::Log(const char tag[], const char format[], ...) {
}
