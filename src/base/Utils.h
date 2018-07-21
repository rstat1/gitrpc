/*
* Copyright (c) 2013 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef UTILFUNCS
#define UTILFUNCS

#include <vector>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <base/base_exports.h>

#if defined(OS_LINUX) || defined(OS_STEAMLINK)
#include <execinfo.h>
#endif

using namespace std;

namespace base { namespace utils
{
	struct CStringComparator
	{
		bool operator()(const char* a, const char* b) const { return strcmp(a, b) < 0; }
	};
	BASEAPI std::string GenerateRandomString(size_t len);
    BASEAPI std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	BASEAPI std::vector<std::string> split(const std::string &s, char delim);
	BASEAPI bool EndsWith(const std::string &str, const std::string &suffix);
	BASEAPI std::string GetPlatformPathPrefix();
	BASEAPI std::string GetAppPath();
	BASEAPI int GetThreadID();
	BASEAPI std::string str_tolower(std::string s);
#if defined(OS_WIN)
	std::string GetLastErrorStdStr();
#endif
#if defined(OS_LINUX) || defined(OS_STEAMLINK)
	BASEAPI void OutputBacktrace();
	BASEAPI pthread_t GetPthreadID();
	BASEAPI int64_t GetUnixTimestamp();
	BASEAPI bool CreateUnixSocket(const char* name);
#endif
}}

#endif