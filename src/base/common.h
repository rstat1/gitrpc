/*
* Copyright (c) 2012-2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/
// clang-format off


#ifndef CONFIG
#define CONFIG
#define BASE_DEBUG 1
#if defined(WITH_SDL)
	#define USING_SDL 1
#endif
#if defined(WITH_UI)
	#define INCLUDE_UI 1
#endif
#if defined(ENABLE_REMOTERY)
	#include <remotery/Remotery.h>
	#define INIT_PROFILER \
		Remotery* rmt;    \
		rmt_CreateGlobalInstance(&rmt);
	#define UNINIT_PROFILER rmt_DestroyGlobalInstance(rmt);
	#define PROFILE_LOG(entry) rmt_LogText(entry)
	#define PROFILE_CPU(name, flags) rmt_ScopedCPUSample(name, flags)
#else
#define INIT_PROFILER
#define INIT_GPU_PROFILE
#define UNINIT_PROFILER
#define PROFILE_LOG(entry)
#define PROFILE_CPU(name, flags)
#define PROFILE_GPU(name)
#endif
#if defined(WINDOWS)
	#include <Windows.h>
	#include <direct.h>
	#define _Platform "Windows"
	#define OS_WIN 1
	#define Instance HINSTANCE
	#define WindowHandle HWND
	#define GetCurrentDir _getcwd
	#ifdef BASE_EXPORTS
		#define BASEAPI __declspec(dllexport)
	#else
		#define BASEAPI __declspec(dllimport)
	#endif
#elif defined(LINUX)
	#define BASEAPI
	#include <unistd.h>
	#if defined(__arm__)
		#define _Platform "rpi"
		#define OS_RPI 1
		#define OS_LINUX 1
		#define INIT_GPU_PROFILE
		#define PROFILE_GPU(name)
	#else
		#define _Platform "Linux"
		#define OS_LINUX 1
		#if defined(ENABLE_REMOTERY)
		#define INIT_GPU_PROFILE rmt_BindOpenGL();
		#define PROFILE_GPU(name) rmt_ScopedOpenGLSample(name)
		#endif
	#endif
	#define Instance long unsigned int
	#define WindowHandle long unsigned int
	#define GetCurrentDir getcwd
#elif defined(__ANDROID__)
	#define BASEAPI
	#define OS_ANDROID 1
	#define _Platform "Android"
#else
	#error This platform is not supported.
#endif
#if defined(__GNUC__)
	#define COMPILER_GCC 1
#elif defined(_MSC_VER)
	#define COMPILER_MSVC 1
#elif defined(__clang__)
	#define COMPILER_CLANG 1
#else
	#error Please add support for your compiler in common.h
#endif
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
	#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#elif defined(COMPILER_MSVC)
#define WARN_UNUSED_RESULT
#endif
#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)
	#define ARCH_CPU_64_BITS 1
#else
	#define ARCH_CPU_32_BITS 1
#endif
#define OVERRIDE override
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)
#endif
#ifdef __cplusplus
#include <base/logging.h>
#include <memory>
#define NOTIMPLEMENTED() LOG_ARGS("%s is not currently implemented.", __FUNCTION__);
#define SINGLETON(name)                                       \
public:                                                       \
	static name* Get() {                                      \
		if (!name::ref) { ref = ::std::make_shared<name>(); } \
		return ref.get();                                     \
	}                                                         \
                                                              \
private:                                                      \
	static ::std::shared_ptr<name> ref;

#define SINGLETON_DEF(name) ::std::shared_ptr<name> name::ref;
#define VEC_ITER(vec) for (auto it = vec.begin(); it != vec.end(); it++)

#endif

// clang-format on