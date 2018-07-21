// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modified 6/6/2017 by rstat1 to work within my existing "base" library.

#include <base/pickle/Pickle.h>

namespace base
{
	PickleIterator::PickleIterator(const Pickle& pickle)
		: payload_(pickle.payload()), read_index_(0), end_index_(pickle.payload_size())
	{}
	template <typename Type>
	inline bool PickleIterator::ReadBuiltinType(Type* result)
	{
		const char* read_from = GetReadPointerAndAdvance<Type>();
		if (!read_from)
			return false;
		if (sizeof(Type) > sizeof(uint32_t))
			memcpy(result, read_from, sizeof(*result));
		else
			*result = *reinterpret_cast<const Type*>(read_from);
		return true;
	}
	inline void PickleIterator::Advance(size_t size)
	{
		size_t aligned_size = AlignInt(size, sizeof(uint32_t));
		if (end_index_ - read_index_ < aligned_size) {
			read_index_ = end_index_;
		} else {
			read_index_ += aligned_size;
		}
	}
	template<typename Type>
	inline const char* PickleIterator::GetReadPointerAndAdvance()
	{
		if (sizeof(Type) > end_index_ - read_index_) {
			read_index_ = end_index_;
			return NULL;
		}
		const char* current_read_ptr = payload_ + read_index_;
		Advance(sizeof(Type));
		return current_read_ptr;
	}
	const char* PickleIterator::GetReadPointerAndAdvance(int num_bytes)
	{
		if (num_bytes < 0 ||
			end_index_ - read_index_ < static_cast<size_t>(num_bytes)) {
			read_index_ = end_index_;
			return NULL;
		}
		const char* current_read_ptr = payload_ + read_index_;
		Advance(num_bytes);
		return current_read_ptr;
	}
	inline const char* PickleIterator::GetReadPointerAndAdvance(int num_elements, size_t size_element)
	{
  		// Check for int32 overflow.
		int64_t num_bytes = static_cast<int64_t>(num_elements) * size_element;
		int num_bytes32 = static_cast<int>(num_bytes);
		if (num_bytes != static_cast<int64_t>(num_bytes32))
			return NULL;
		return GetReadPointerAndAdvance(num_bytes32);
	}
	bool PickleIterator::ReadBool(bool* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadInt(int* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadLong(long* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadUInt16(uint16_t* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadUInt32(uint32_t* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadInt64(int64_t* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadUInt64(uint64_t* result)
	{
		return ReadBuiltinType(result);
	}
	bool PickleIterator::ReadFloat(float* result)
	{
		// crbug.com/315213
		// The source data may not be properly aligned, and unaligned float reads
		// cause SIGBUS on some ARM platforms, so force using memcpy to copy the data
		// into the result.
		const char* read_from = GetReadPointerAndAdvance<float>();
		if (!read_from)
			return false;
		memcpy(result, read_from, sizeof(*result));
		return true;
	}
	bool PickleIterator::ReadDouble(double* result)
	{
		// crbug.com/315213
		// The source data may not be properly aligned, and unaligned double reads
		// cause SIGBUS on some ARM platforms, so force using memcpy to copy the data
		// into the result.
		const char* read_from = GetReadPointerAndAdvance<double>();
		if (!read_from)
			return false;
		memcpy(result, read_from, sizeof(*result));
		return true;
	}
	bool PickleIterator::ReadString(std::string* result)
	{
		int len;
		if (!ReadInt(&len))
			return false;
		const char* read_from = GetReadPointerAndAdvance(len);
		if (!read_from)
			return false;

		result->assign(read_from, len);
		return true;
	}
	bool PickleIterator::ReadWString(std::wstring* result)
	{
		int len;
		if (!ReadInt(&len))
			return false;
		const char* read_from = GetReadPointerAndAdvance(len, sizeof(wchar_t));
		if (!read_from)
			return false;

		result->assign(reinterpret_cast<const wchar_t*>(read_from), len);
		return true;
	}
	bool PickleIterator::ReadData(const char** data, int* length)
	{
		*length = 0;
		*data = 0;
		if (!ReadInt(length))
			return false;
		return ReadBytes(data, *length);
	}
	bool PickleIterator::ReadBytes(const char** data, int length)
	{
		const char* read_from = GetReadPointerAndAdvance(length);
		if (!read_from)
			return false;
		*data = read_from;
		return true;
	}
}