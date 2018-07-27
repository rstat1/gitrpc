/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GITSVCCOM
#define GITSVCCOM

#include <protocols/generated/GitService.grpc.pb.h>

namespace nexus { namespace common {
	#define SERVER_SOCKET "/tmp/nexus_git_service"
	#define VF std::function<void()>
	#define REPO_PATH(name) std::string newRepoPath(DEFAULT_REPO_PATH);\
							newRepoPath.append(name);
	#define CFE(func, msg, onsuccess) if (strcmp(this->CheckForError(func, msg), "") == 0) { \
										onsuccess(); \
										response->set_success(true); \
										response->set_errormessage("Success!"); \
									  } else {\
									  	response->set_success(false); \
										response->set_errormessage(msg); \
									  }

	#define OPEN_REPO(name) git_repository* repo; \
							CFE(git_repository_open(&repo, this->ConvertRepoNameToPath(name)), \
								"Failed to open repo");
}}

#endif