/*
* Copyright (c) 2014 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef THRDLCLSTOR
#define THRDLCLSTOR

#include <base/threading/common/thread_types.h>

namespace base { namespace threading {
	class ThreadLocalStorage {
	public:
		static void AllocateTLSSlot(SlotType *slot);
		static void FreeTLSSlot(SlotType slot);
		static void SetTLSSlotValue(void *value, SlotType slot);
		static void *GetTLSSlotValue(SlotType slot);
	};
}} // namespace base::threading

#endif