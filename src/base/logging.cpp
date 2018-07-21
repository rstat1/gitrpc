/*
* Copyright (c) 2012 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <base/common.h>
#include <stdarg.h>

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

const char* Logging::tagToShow = "";

void writeToLog(const char* entry, bool deleteFile)
{
	Logging::writeToLog(entry, deleteFile, true);
}
void writeToLog(const char* entry)
{
	Logging::writeToLog(entry, false, true);
}
void Log(const char* tag, int line, const char format[], ...)
{
#if _DEBUG
    va_list args;
    va_start(args, format);

#if defined(OS_ANDROID) || defined(MOBILEBOT)
    __android_log_vprint(ANDROID_LOG_DEBUG, tag, format, args);
#else
	static const size_t kBufferSize = 2048;
   	char buffer[kBufferSize + 1];
	const char* TAG = tag;
	if (Logging::tagToShow == TAG || Logging::tagToShow == "")
	{
		vsnprintf(buffer, kBufferSize, format, args);
	}
   	va_end(args);
	if (Logging::tagToShow == tag || Logging::tagToShow == "")
	{
		string logLine("");
		logLine.append(tag);
		logLine.append(":");
		logLine.append(to_string(line));
		logLine.append("] ");
		logLine.append(buffer);
		Logging::writeToLog(logLine.c_str(), false, false);
	}
#endif
#endif
}
void LogShowOnlyTag(const char* tag)
{
	Logging::ShowOnlyTag(tag);
}
void MonoLog(const char* entry, bool deleteFile)
{
	Logging::MonoLog(entry, deleteFile);
}

void Logging::writeToLog(const char* entry, bool deleteFile, bool autoAppendTag)
{
#if !defined(OS_ANDROID) && _DEBUG
	FILE * pFile;

	string logEntry("");
	logEntry.append("[thread:");
	logEntry.append(std::to_string(GetThreadID()));
	logEntry.append(",process:");
	// logEntry.append("");
	logEntry.append(program_invocation_short_name);
	logEntry.append(",file:");
	if (autoAppendTag) { logEntry.append("All] "); }
	logEntry.append(entry);

	if (deleteFile == true) {pFile = fopen("armite-log.txt", "w");}
	else {pFile = fopen("armite-log.txt", "a+");}

	if (pFile != NULL )
	{
		fputs(logEntry.c_str(), pFile);
		fputc('\n', pFile);

		fclose(pFile);
	}
#elif DEBUG
	LOG(INFO, TAG, entry);
#endif
}
void Logging::writeToLog(const char* entry)
{
	writeToLog(entry, false, true);
}
void Logging::ShowOnlyTag(const char* tag)
{
	Logging::tagToShow = tag;
}
void Logging::MonoLog(const char* entry, bool deleteFirst)
{
#if _DEBUG
	FILE * pFile;

	string logEntry("");
	logEntry.append(entry);

	if (deleteFirst == true) { pFile = fopen("monologue.txt", "w"); }
	else { pFile = fopen("monologue.txt", "a+"); }

	if (pFile != NULL)
	{
		fputs(logEntry.c_str(), pFile);
		fputc('\n', pFile);

		fclose(pFile);
	}
#endif
}
void Logging::Log(const char tag[], const char format[], ...)
{

}
