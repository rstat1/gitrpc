/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef MEMTRK
#define MEMTRK

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <base/common.h>
#include <sys/resource.h>

using namespace std;

namespace base { namespace platform
{
	class MemTrack
	{
		public:
			static double GetPeakMemoryUse();
			static double GetCurrentMemoryUse();
	};
}}

#endif