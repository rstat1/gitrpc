/*
* Copyright (c) 2018 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GITSVCCOM
#define GITSVCCOM

#include <git/git2.h>
#include <protocols/generated/GitService.grpc.pb.h>
#define SERVER_SOCKET "nexus_git_service"
#define SUCCESS std::function<void()>
#define REPO_PATH(name) std::string newRepoPath(DEFAULT_REPO_PATH);\
						newRepoPath.append(name);
enum class RequestStatus {
	READ = 0,
	WRITE,
	CONNECT,
	DONE,
	FINISH,
};
enum class RequestType {
	GIT_PUSH,
	GIT_CLONE,
	UNKNOWN
};
class Common {
	public:
		static const char* CheckForError(int errCode, const char* message) {
			const git_error* err;
			if (errCode == 0) {	return ""; }
			else {
				err = giterr_last();
				if (err != nullptr && err->message != NULL) {
					LOG_ARGS("error %s: %s", message, err->message);
					return err->message;
				}
			}
			return "";
	}
};

#endif