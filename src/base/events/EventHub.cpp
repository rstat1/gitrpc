/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#include <base/events/EventHub.h>
#include <base/threading/dispatcher/DispatcherTypes.h>
#include <memory>

using namespace base::threading;

namespace base { namespace events {
	std::shared_ptr<EventHub> EventHub::ref;

	void EventHub::RegisterEvent(const char *name) {
		this->eventHandlers.insert(std::make_pair(name, EventHandlers()));
	}
	void EventHub::RegisterEventHandler(const char *name, EventHandler handler) {
		if (this->eventHandlers.find(name) != this->eventHandlers.end()) {
			this->eventHandlers[name].push_back(handler);
		} else {
			LOG_ARGS("Unrecognized event %s", name);
		}
	}
	void EventHub::TriggerEvent(const char *name, void *args) {
		if (this->eventHandlers.find(name) != this->eventHandlers.end()) {
			EventHandlers handlers = this->eventHandlers[name];
			for (EventHandler eh : handlers) {
				if (eh.runHandlerAsTask) {
					ThreadedEventHandlerArgs *teArgs = new ThreadedEventHandlerArgs(eh, std::move(args));
					NEW_TASK1(EventDispatch, EventHub, EventHub::Get(), EventHub::RunEventHandler, teArgs);
					POST_TASK(EventDispatch, eh.owningThread);
				} else {
					eh.handler(args);
				}
			}
		} else {
			LOG_ARGS("Unrecognized event %s", name);
		}
	}
	void EventHub::RunEventHandler(void *args) {
		ThreadedEventHandlerArgs *eventArgs = (ThreadedEventHandlerArgs *)args;
		eventArgs->eventHandler.handler(eventArgs->args);
	}
	void EventHub::UnregisterEventHandler(const char *name, std::string id) {
		auto handlers = this->eventHandlers[name];
		auto h = std::find_if(handlers.begin(), handlers.end(), [&](EventHandler eh) {
			return eh.id == id;
		});
		if (h != handlers.end()) { handlers.erase(h); }
		this->eventHandlers[name] = handlers;
	}
}} // namespace base::events