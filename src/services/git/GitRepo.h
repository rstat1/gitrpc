/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GITRPO
#define GITRPO

#include <string>
#include <common/GitServiceCommon.h>

#define DEFAULT_REPO_PATH "/home/rstat1/Apps/test/"

namespace nexus { namespace git {
    class GitRepo {
		public:
			GitRepo(std::string name);
			~GitRepo();
			const char* Open(bool bare);
			git_odb_writepack* WritePack();
            static int TransferProgressCB(const git_transfer_progress *stats, void *payload);
		private:
            git_odb* odb;
			std::string repoName;
			git_repository* repo;
	};
}}

#endif