/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef EVNTHUB
#define EVNTHUB

#include <base/Utils.h>
#include <base/common.h>
#include <base/events/EventArgMacros.h>
#include <functional>
#include <map>
#include <vector>

#define HANDLER [&](void *args)
#define HANDLE_EVENT(name, onThread, handler)                        \
	base::events::EventHandler name(true, onThread, handler, #name); \
	base::events::EventHub::Get()->RegisterEventHandler(#name, name);
#define TRIGGER_EVENT(name, args) base::events::EventHub::Get()->TriggerEvent(#name, args);
#define EVENT(name) base::events::EventHub::Get()->RegisterEvent(#name);

namespace base { namespace events {
	struct EventHandler {
	public:
		const char *event = "";
		bool runHandlerAsTask = false;
		std::function<void(void *)> handler;
		const char *owningThread = "Main";
		std::string id;
		EventHandler(bool runAsTask, const char *onThread, std::function<void(void *)> handlerFunc,
					 const char *event) : runHandlerAsTask(runAsTask), owningThread(onThread), handler(handlerFunc), event(event) {
			id = base::utils::GenerateRandomString(16);
		}
	};
	typedef std::vector<EventHandler> EventHandlers;
	struct ThreadedEventHandlerArgs : std::enable_shared_from_this<ThreadedEventHandlerArgs> {
	public:
		EventHandler eventHandler;
		void *args;
		ThreadedEventHandlerArgs(EventHandler handler, void *eventArgs) : eventHandler(handler), args(eventArgs) {}
	};
	class EventHub {
	public:
		void RunEventHandler(void *args);
		void RegisterEvent(const char *name);
		void TriggerEvent(const char *name, void *args);
		void RegisterEventHandler(const char *name, EventHandler handler);
		void UnregisterEventHandler(const char *name, std::string id);

	private:
		std::map<const char *, EventHandlers, base::utils::CStringComparator> eventHandlers;

		SINGLETON(EventHub)
	};
}} // namespace base::events

#endif