/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef REPOMAN
#define REPOMAN

#include <atomic>
#include <future>
#include <mutex>

#include <base/common.h>
#include <services/git/repository/GitRepo.h>

namespace gitrpc { namespace git {
	using namespace nexus::git;

	class RepositoryManager {
	public:
		void Init();
		void CloseRepo();
		void OpenRepo(void* data);
		void GetRepoRefs(void* data);
		void NewReference(void* data);
		void AppendDataToPack(void* data);
		void CommitPackChanges(void* data);

	private:
		GitRepo* currentRepo;
		std::mutex repoLocker;
		std::atomic<int> repoRefCnt;
		std::promise<bool> packCommitComplete;

		SINGLETON(RepositoryManager);
	};
}} // namespace gitrpc::git

#endif