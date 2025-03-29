#pragma once

namespace CW_E {

	class OnEvent
	{
	public:
		virtual ~OnEvent() = default;
		virtual bool isAcceptingEvents() const { return true; }
	};

} // CW_E