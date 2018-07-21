// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modified 6/6/2017 by rstat1 to work within my existing "base" library.

#include <base/pickle/Pickle.h>

namespace base
{
	const int Pickle::kPayloadUnit = 64;
	static const size_t kCapacityReadOnly = static_cast<size_t>(-1);
	Pickle::Pickle()
    	: header_(NULL), header_size_(sizeof(Header)), capacity_after_header_(0), write_offset_(0)
	{
		Resize(kPayloadUnit);
		header_->payload_size = 0;
	}
	Pickle::Pickle(int header_size)
    	: header_(NULL),
      	header_size_(AlignInt(header_size, sizeof(uint32_t))),
      	capacity_after_header_(0),
      	write_offset_(0)
	{
		DCHECK_GE(static_cast<size_t>(header_size), sizeof(Header));
  		DCHECK_LE(header_size, kPayloadUnit);
  		Resize(kPayloadUnit);
  		header_->payload_size = 0;
	}
	Pickle::Pickle(const char* data, int data_len)
		: header_(reinterpret_cast<Header*>(const_cast<char*>(data))),
		header_size_(0),
		capacity_after_header_(kCapacityReadOnly),
		write_offset_(0)
	{
		if (data_len >= static_cast<int>(sizeof(Header)))
			header_size_ = data_len - header_->payload_size;

		if (header_size_ > static_cast<unsigned int>(data_len))
			header_size_ = 0;

		if (header_size_ != AlignInt(header_size_, sizeof(uint32_t)))
			header_size_ = 0;

		// If there is anything wrong with the data, we're not going to use it.
		if (!header_size_)
			header_ = NULL;
	}
	Pickle::Pickle(const Pickle& other)
		: header_(NULL),
		header_size_(other.header_size_),
		capacity_after_header_(0),
		write_offset_(other.write_offset_)
	{
  		size_t payload_size = header_size_ + other.header_->payload_size;
  		Resize(payload_size);
  		memcpy(header_, other.header_, payload_size);
	}
	Pickle::~Pickle()
	{
 		if (capacity_after_header_ != kCapacityReadOnly)
    	free(header_);
	}
	Pickle& Pickle::operator=(const Pickle& other)
	{
		if (this == &other) {
			NOTREACHED();
			return *this;
		}
		if (capacity_after_header_ == kCapacityReadOnly) {
			header_ = NULL;
			capacity_after_header_ = 0;
		}
		if (header_size_ != other.header_size_) {
			free(header_);
			header_ = NULL;
			header_size_ = other.header_size_;
		}
		Resize(other.header_->payload_size);
		memcpy(header_, other.header_,
				other.header_size_ + other.header_->payload_size);
		write_offset_ = other.write_offset_;
		return *this;
	}
	bool Pickle::WriteString(const std::string& value)
	{
		if (!WriteInt(static_cast<int>(value.size())))
			return false;

		return WriteBytes(value.data(), static_cast<int>(value.size()));
	}
	bool Pickle::WriteWString(const std::wstring& value)
	{
		if (!WriteInt(static_cast<int>(value.size())))
			return false;

		return WriteBytes(value.data(), static_cast<int>(value.size() * sizeof(wchar_t)));
	}
	bool Pickle::WriteData(const char* data, int length)
	{
  		return length >= 0 && WriteInt(length) && WriteBytes(data, length);
	}
	bool Pickle::WriteBytes(const void* data, int length)
	{
  		WriteBytesCommon(data, length);
  		return true;
	}
	void Pickle::Reserve(size_t length)
	{
		size_t data_len = AlignInt(length, sizeof(uint32_t));
		DCHECK_GE(data_len, length);
		#ifdef ARCH_CPU_64_BITS
			DCHECK_LE(data_len, kuint32max);
		#endif
		DCHECK_LE(write_offset_, kuint32max - data_len);
		size_t new_size = write_offset_ + data_len;
		if (new_size > capacity_after_header_)
			Resize(capacity_after_header_ * 2 + new_size);
	}
	void Pickle::Resize(size_t new_capacity)
	{
		new_capacity = AlignInt(new_capacity, kPayloadUnit);

		CHECK_NE(capacity_after_header_, kCapacityReadOnly);
		void* p = realloc(header_, header_size_ + new_capacity);
		CHECK(p);
		header_ = reinterpret_cast<Header*>(p);
		capacity_after_header_ = new_capacity;
	}
	const char* Pickle::FindNext(size_t header_size, const char* start, const char* end)
	{
		DCHECK_EQ(header_size, AlignInt(header_size, sizeof(uint32)));
		DCHECK_LE(header_size, static_cast<size_t>(kPayloadUnit));

		size_t length = static_cast<size_t>(end - start);
		if (length < sizeof(Header))
			return NULL;

		const Header* hdr = reinterpret_cast<const Header*>(start);
		if (length < header_size || length - header_size < hdr->payload_size)
			return NULL;
		return start + header_size + hdr->payload_size;
	}
	template <size_t length> void Pickle::WriteBytesStatic(const void* data)
	{
  		WriteBytesCommon(data, length);
	}
	template void Pickle::WriteBytesStatic<2>(const void* data);
	template void Pickle::WriteBytesStatic<4>(const void* data);
	template void Pickle::WriteBytesStatic<8>(const void* data);
	inline void Pickle::WriteBytesCommon(const void* data, size_t length)
	{
		size_t data_len = AlignInt(length, sizeof(uint32_t));
		DCHECK_GE(data_len, length);
		#ifdef ARCH_CPU_64_BITS
		DCHECK_LE(data_len, kuint32max);
		#endif
		DCHECK_LE(write_offset_, kuint32max - data_len);
		size_t new_size = write_offset_ + data_len;
		if (new_size > capacity_after_header_) {
			Resize(std::max(capacity_after_header_ * 2, new_size));
  		}

		char* write = mutable_payload() + write_offset_;
		memcpy(write, data, length);
		memset(write + length, 0, data_len - length);
		header_->payload_size = static_cast<uint32_t>(new_size);
		write_offset_ = new_size;
	}
}