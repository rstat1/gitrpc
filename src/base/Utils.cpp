/*
* Copyright (c) 2013 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/Utils.h>
#include <base/common.h>
#include <filesystem>
#include <random>
#if defined(OS_WIN)
#include <windows.h>
#else
#include <errno.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/un.h>
#endif

namespace base { namespace utils {
	using namespace std::filesystem;
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}
	std::string GetAppPath() {
		return current_path().string();
	}
	float RandomNumber() {
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::uniform_real_distribution<float> dist(0, 1);
		return dist(rng);
	}
	int RandomInt(int max) {
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::uniform_int_distribution<int> dist(0, max);
		return dist(rng);
	}
	//modified version of this SO answer https://stackoverflow.com/a/19515939/342763
	std::string GenerateRandomString(size_t len) {
		const char alphabet[] =
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"0123456789";

		size_t N_STRS = 1;
		size_t S_LEN = len;

		std::random_device rd;
		std::default_random_engine rng(rd());
		std::uniform_int_distribution<> dist(0, sizeof(alphabet) / sizeof(*alphabet) - 2);

		std::string str;
		str.reserve(S_LEN);
		std::generate_n(std::back_inserter(str), S_LEN, [&] {
			return alphabet[dist(rng)];
		});
		return str;
	}
	std::string GetPlatformPathPrefix() {
#if defined(OS_WIN)
		return "";
#elif defined(OS_ANDROID)
		return "/data/data/us.rdro.xpappfx/";
#else
		return "";
#endif
	}
	int GetThreadID() {
#if defined(OS_WIN)
		return (int)GetCurrentThreadId();
#else
		return (int)syscall(__NR_gettid);
#endif
	}
#if defined(OS_LINUX) || defined(OS_ANDROID)
	pthread_t GetPthreadID() {
		return pthread_self();
	}
#endif
	//https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt
	bool EndsWith(const std::string &str, const std::string &suffix) {
		return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}
#if defined(OS_WIN)
	std::string GetLastErrorStdStr() {
		DWORD error = GetLastError();
		if (error) {
			LPVOID lpMsgBuf;
			DWORD bufLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error,
										 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
			if (bufLen) {
				LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
				std::string result(lpMsgStr, lpMsgStr + bufLen);

				LocalFree(lpMsgBuf);

				return result;
			}
		}
		return std::string();
	}
#endif
	std::string str_tolower(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
			return std::tolower(c);
		});
		return s;
	}
#if defined(OS_LINUX) || defined(OS_STEAMLINK)
	void OutputBacktrace() {
		int j, nptrs;
		void *buffer[100];
		char **strings;

		nptrs = backtrace(buffer, 100);
		strings = backtrace_symbols(buffer, nptrs);
		if (strings == NULL) {
			writeToLog("Failed to get backtrace");
			return;
		}

		for (j = 0; j < nptrs; j++) {
			LOG_ARGS("%s\n", strings[j]);
		}

		free(strings);
	}
	bool CreateUnixSocket(const char *name) {
		struct sockaddr_un addr;
		LOG_ARGS("make unix socket named %s", name);
		int socketFD = socket(AF_UNIX, SOCK_STREAM, 0);
		if (socketFD != -1) {
			memset(&addr, 0, sizeof(struct sockaddr_un));
			addr.sun_family = AF_UNIX;
			strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);
			int ret = bind(socketFD, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
			if (ret != -1) {
				return true;
			} else {
				LOG_ARGS("failed %s", strerror(errno));
				return false;
			}
		} else {
			LOG_ARGS("failed %s", strerror(errno));
			return false;
		}
	}
	int64_t GetUnixTimestamp() {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
	}
#endif
}} // namespace base::utils