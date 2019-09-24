/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MEMTRK
#define MEMTRK

#include <base/common.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace std;

namespace base { namespace platform {
	class MemTrack {
	public:
		static double GetPeakMemoryUse();
		static double GetCurrentMemoryUse();
	};
}} // namespace base::platform

#endif