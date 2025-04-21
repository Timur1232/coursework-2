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
		const T* GetIf() const
		{
			return std::get_if<T>(&data);
		}
	};


	template<class Target_t, class Event_t>
	class UserDispatcher
	{
	public:
		UserDispatcher(OnEvent* target, const MyEvent& event)
			: m_Target(dynamic_cast<Target_t*>(target)), m_Event(event.GetIf<Event_t>())
		{
		}

		bool IsValid() const
		{
			bool valid = m_Target && m_Event;
			if (valid) valid = m_Target->IsAcceptingEvents();
			return valid;
		}

		void operator()()
		{
			CW_ERROR("Need overloading for this type of event!");
		}

	private:
		Target_t* m_Target = nullptr;
		const Event_t* m_Event = nullptr;
	};

#endif

} // CW