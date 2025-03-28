#pragma once

namespace CW {

	class OnEvent
	{
	public:
		virtual ~OnEvent() = default;
		virtual bool isAcceptingEvents() const { return true; }
	};

} // CW