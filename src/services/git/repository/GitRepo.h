/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef GITRPO
#define GITRPO

#include <string>
#include <vector>

#include <base/PropertyMacros.h>
#include <common/GitServiceCommon.h>

#define DEFAULT_REPO_PATH "/home/rstat1/Apps/test/"

namespace gitrpc { namespace git {
	struct ReferenceInfo;
	struct GetRefsResponse;
}} // namespace gitrpc::git

namespace nexus { namespace git {
	class GitRepo {
	public:
		GitRepo(std::string name);
		~GitRepo();
		const char* Open(bool bare);
		const char* CreateNewPackBuilder();
		gitrpc::git::GetRefsResponse GetRepoReferences();
		const char* PackCommit(git_transfer_progress* stats);
		const char* CreateReference(const char* refRev, const char* refName);
		const char* PackAppend(const void* data, size_t size, git_transfer_progress* stats);

		static int TransferProgressCB(const git_transfer_progress* stats, void* payload);

	private:
		const char* InitWritePackFunctions();

		git_odb* odb;
		std::string repoPath;
		git_repository* repo;
		git_odb_writepack* wp;
		git_transfer_progress tStats;

		PROPERTY(Name, std::string);
	};
}} // namespace nexus::git

#endif