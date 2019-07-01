/*
* Copyright (c) 2014 The Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <Windows.h>
#include <assert.h>
#include <base/threading/tls/ThreadLocalStorage.h>

namespace base { namespace threading {
	void ThreadLocalStorage::AllocateTLSSlot(SlotType *slot) {
		*slot = TlsAlloc();
	}
	void ThreadLocalStorage::FreeTLSSlot(SlotType slot) {
		assert(TlsFree(slot) != false);
	}
	void *ThreadLocalStorage::GetTLSSlotValue(SlotType slot) {
		return TlsGetValue(slot);
	}
	void ThreadLocalStorage::SetTLSSlotValue(void *value, SlotType slot) {
		assert(TlsSetValue(slot, value) != false);
	}
}} // namespace base::threading