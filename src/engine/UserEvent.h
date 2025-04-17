#pragma once

#include "pch.h"

#include "CW_EventConfig.h"
#include "debug_utils/Log.h"

namespace CW {

#ifdef CW_USER_EVENTS_LIST

	struct MyEvent
	{
		std::variant<CW_USER_EVENTS_LIST> data;

		template<class T>
		const T* getIf() const
		{
			return std::get_if<T>(&data);
		}
	};


	template<class Target_t, class Event_t>
	class UserDispatcher
	{
	public:
		UserDispatcher(OnEvent* target, const MyEvent& event)
			: target(dynamic_cast<Target_t*>(target)), event(event.getIf<Event_t>())
		{
		}

		bool isValid() const
		{
			bool valid = target && event;
			if (valid) valid = target->isAcceptingEvents();
			return valid;
		}

		void operator()()
		{
			CW_ERROR("Need overloading for this type of event!");
		}

	private:
		Target_t* target = nullptr;
		const Event_t* event = nullptr;
	};

#endif

} // CW