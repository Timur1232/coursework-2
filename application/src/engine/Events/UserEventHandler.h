#pragma once
#include "pch.h"

#include "debug_utils/Log.h"
#include "UserEvents.h"
#include "engine/Application.h"

#include "CW_EventConfig.h"

namespace CW {

	struct MyEvent
	{
		std::variant<CW_USER_EVENTS_LIST> Data;

		template<class T>
		constexpr const T* GetIf() const
		{
			return std::get_if<T>(&Data);
		}

		template<class T>
		constexpr bool Is() const
		{
			return std::holds_alternative<T>(&Data);
		}
	};

	class UserEventHandler
	{
	public:
		UserEventHandler(const UserEventHandler&) = delete;
		UserEventHandler(UserEventHandler&&) = delete;

		static UserEventHandler& Get();

		void Reserve(size_t eventReserve);

		template<class T>
		void SendEvent(T&& event)
		{
			m_UserEvents.emplace_back(MyEvent{ .Data = std::move(event) });
		}

		std::vector<MyEvent>& GetEvents() { return m_UserEvents; }
		void ClearEvents() { m_UserEvents.clear(); }

	private:
		UserEventHandler() = default;

	private:
		std::vector<MyEvent> m_UserEvents;
	};



	template <class _EventType>
	class UserEventDispatcher
	{
	public:
		UserEventDispatcher(const MyEvent& event)
			: m_Event(event.GetIf<_EventType>())
		{
		}

		constexpr operator bool() const
		{
			return m_Event != nullptr;
		}

		void operator()(Application& app)
		{
			_EventType event = *m_Event;
			app.OnEvent(event);
		}

	private:
		const _EventType* m_Event;
	};

} // CW