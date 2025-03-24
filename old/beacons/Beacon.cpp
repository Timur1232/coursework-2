#include "Beacon.h"

namespace CW
{

	void Beacon::update(uint64_t currentTick)
	{
		--m_FadingTimer;
		if (m_FadingTimer == 0)
		{
			m_Alive = false;
		}
	}

} // CW