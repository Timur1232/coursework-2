#pragma once

namespace CW_E {

	class OnEvent
	{
	public:
		virtual ~OnEvent() = default;
		virtual bool isAcceptingEvents() const;

		void subscribeOnEvents();
		void unsubscribeOnEvents() const;

		size_t getIndex() const;
		void setIndex(size_t index);

	private:
		size_t m_EventRecieverIndex = 0;
	};

} // CW_E