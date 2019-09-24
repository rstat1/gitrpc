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
#include <base/threading/dispatcher/DispatcherTypes.h>
#include <services/git/repository/GitRepo.h>

namespace gitrpc { namespace git {
	using namespace nexus::git;
	using namespace base::threading;
	struct OpenRepoArgs {
		const char* repoName;
		std::promise<std::string> result;
	};
	struct WritePackDataArgs {
		size_t size;
		const void* data;
		std::promise<std::string> result;
	};
	struct GenericArgs {
		std::promise<std::string> result;
	};
	struct NewReferenceArgs {
		std::string refName;
		std::string refRev;
		std::promise<std::string> result;
	};
	struct WaitForCommitArgs {
		std::promise<std::future<bool>> result;
	};
	class RepoProxy {
	public:
		void CloseRepo();
		std::future<std::string> PackCommit();
		std::future<std::string> OpenRepo(std::string name);
		std::future<std::string> PackAppend(const void* data, size_t size);
		std::future<std::string> CreateReference(std::string refName, std::string refRev);
		SINGLETON(RepoProxy);
	};
	class RepositoryManager {
	public:
		void Init();
		void CloseRepo();
		void OpenRepo(void* data);
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