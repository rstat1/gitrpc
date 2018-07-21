/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GSAPPIMPL
#define GSAPPIMPL

#include <gitserver/app/App.h>

namespace nexus {
    class AppImpl : public App
	{
		public:
			AppImpl(): App(std::bind(&AppImpl::Main, this)) {
				LOG_FROM_HERE_E("init TranscodeServer");
			}
			void Main();
		private:
    };
}

#endif