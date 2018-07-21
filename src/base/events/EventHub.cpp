/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <memory>
#include <base/events/EventHub.h>
#include <base/threading/dispatcher/DispatcherTypes.h>

using namespace base::threading;

namespace base { namespace events
{
	std::shared_ptr<EventHub> EventHub::ref;

	void EventHub::RegisterEvent(const char* name)
	{
		if (this->eventHandlers.find(name) == this->eventHandlers.end())
		{
			this->eventHandlers.insert(std::make_pair(name, EventHandlers()));
		}
	}
	void EventHub::RegisterEventHandler(const char* name, EventHandler handler)
	{
		this->eventHandlers[name].push_back(handler);
	}
	void EventHub::TriggerEvent(const char* name, void* args)
	{
		if (this->eventHandlers.find(name) != this->eventHandlers.end())
		{
			EventHandlers handlers = this->eventHandlers[name];
			for (EventHandler eh : handlers)
			{
				if (eh.runHandlerAsTask)
				{
					ThreadedEventHandlerArgs *teArgs = new ThreadedEventHandlerArgs(eh, std::move(args));
					NEW_TASK1(EventDispatch, EventHub, EventHub::Get(), EventHub::RunEventHandler, teArgs);
					POST_TASK(EventDispatch, eh.owningThread);
				}
				else { eh.handler(args); }
			}
		}
	}
	void EventHub::RunEventHandler(void *args)
	{
		ThreadedEventHandlerArgs* eventArgs = (ThreadedEventHandlerArgs*)args;
		eventArgs->eventHandler.handler(eventArgs->args);
	}
}}