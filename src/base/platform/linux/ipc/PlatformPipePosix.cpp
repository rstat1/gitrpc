/*
* Copyright (c) 2016 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

#include <base/ipc/PlatformPipe.h>

namespace base { namespace ipc
{
    PlatformChannelPair::PlatformChannelPair()
	{
		int result;

		result = socketpair(AF_UNIX, SOCK_STREAM, 0, this->pipes);
		if (result == -1)
		{
			perror("PlatformPipePosix");
			LOG_FROM_HERE("socketpair could create not. Result: %i", errno);
		}
		else
		{
			if (fcntl(this->pipes[0], F_SETFL, O_NONBLOCK) != 0) { writeToLog("fcntl couldn't. (1)"); }
			if (fcntl(this->pipes[1], F_SETFL, O_NONBLOCK) != 0) { writeToLog("fcntl couldn't. (2)"); }
		}
    }
}}