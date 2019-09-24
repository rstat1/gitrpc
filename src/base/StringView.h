/*
* Copyright (c) 2019 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef STRNVW
#define STRNVW

#include <string>
#include <string_view>

namespace base {
	//Wraps a std::string_view and provides a method for converting substrs
	//of that std::string_view in to something usuable.
	class StringView {
	public:
		StringView(const char* content) : wrapped(content) {}
		StringView(std::string_view toWrap) : wrapped(std::move(toWrap)) {}
		std::string as_string() { return std::string(wrapped.data(), wrapped.size()); }
		const char* as_c_string() { return std::string(wrapped.data(), wrapped.size()).c_str(); }
		constexpr int find(char needle, int pos) { return wrapped.find(needle, pos); }
	private:
		std::string_view wrapped;
	};
} // namespace base

#endif