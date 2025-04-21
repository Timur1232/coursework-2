#pragma once

#include "pch.h"

namespace CW {

	class OnEvent
	{
	public:
		virtual ~OnEvent() = default;
		virtual bool IsAcceptingEvents() const;

		void SubscribeOnEvents();
		void UnsubscribeOnEvents() const;

		size_t GetIndex() const;
		void SetIndex(size_t index);

	private:
		size_t m_EventRecieverIndex = 0;
	};

} // CW