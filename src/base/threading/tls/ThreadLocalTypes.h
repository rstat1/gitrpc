/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/threading/ThreadLocalStorage.h>

namespace base { namespace threading {
	template <typename Type>
	class ThreadLocalPointer {
	public:
		ThreadLocalPointer() : slot() { ThreadLocalStorage::AllocateTLSSlot(&slot); }
		~ThreadLocalPointer() { ThreadLocalStorage::FreeTLSSlot(slot); }
		Type *Get() { return static_cast<Type *>(ThreadLocalStorage::GetTLSSlotValue(slot)); }
		void Set(void *ptr) { ThreadLocalStorage::SetTLSSlotValue(const_cast<void *>(static_cast<const void *>(ptr)), slot); }

	private:
		SlotType slot;
	};
}} // namespace base::threading