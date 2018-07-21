/*
* Copyright (c) 2017 An Incredibly Big Red Robot
*
* Use of this source code is governed by a "BSD-style" license that can be
* found in the included LICENSE file.
*/

#ifndef EVNTHUB
#define EVNTHUB

#include <map>
#include <vector>
#include <functional>
#include <base/Utils.h>
#include <base/common.h>
#include <base/events/EventArgMacros.h>

#define HANDLER [&](void* args)
#define HANDLE_EVENT(name, asTask, onThread, handler) base::events::EventHandler name(asTask, onThread, handler); \
			base::events::EventHub::Get()->RegisterEventHandler(#name, name);
#define TRIGGER_EVENT(name, args) base::events::EventHub::Get()->TriggerEvent(#name, args);
#define EVENT(name) base::events::EventHub::Get()->RegisterEvent(#name);

namespace base { namespace events
{
	struct EventHandler
	{
		public:
			bool runHandlerAsTask = false;
			const char* owningThread = "PlayerApp";
			std::function<void(void*)> handler;
			EventHandler(bool runAsTask, const char* onThread, std::function<void(void*)> handlerFunc) :
				runHandlerAsTask(runAsTask), owningThread(onThread), handler(handlerFunc)
			{}
	};
	typedef std::vector<EventHandler> EventHandlers;
	struct ThreadedEventHandlerArgs : std::enable_shared_from_this<ThreadedEventHandlerArgs>
	{
		public:
			EventHandler eventHandler;
			void* args;
			ThreadedEventHandlerArgs(EventHandler handler, void* eventArgs) :
				eventHandler(handler), args(eventArgs)
			{}
	};
	class EventHub
	{
		public:
			void RunEventHandler(void* args);
			void RegisterEvent(const char* name);
			void TriggerEvent(const char* name, void* args);
			void RegisterEventHandler(const char* name, EventHandler handler);
		private:
			std::map<const char*, EventHandlers, base::utils::CStringComparator> eventHandlers;

		SINGLETON(EventHub)
	};
}}

#endif