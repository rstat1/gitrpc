/*
* Copyright (c) 2013 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/common.h>

#if defined(OS_WIN)
	#ifdef SLA_EXPORTS
		#define SKYLIGHTAPI __declspec(dllexport)
	#else
		#define SKYLIGHTAPI __declspec(dllimport)
	#endif
#else
	#define SKYLIGHTAPI
#endif