/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef REPROX
#define REPROX

#include <future>
#include <vector>

#include <base/common.h>

namespace gitrpc { namespace git {
	struct GetRefsResponse;
	class RepoProxy {
	public:
		void CloseRepo();
		std::future<std::string> PackCommit();
		std::future<std::string> OpenRepo(std::string name);
		std::future<GetRefsResponse> GetReferences();
		std::future<std::string> PackAppend(const void* data, size_t size);
		std::future<std::string> CreateReference(std::string refName, std::string refRev);

		SINGLETON(RepoProxy);
	};
}} // namespace gitrpc::git

#endif