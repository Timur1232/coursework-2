#pragma once

#include <Window.h>

namespace CW
{

	class ISimulationObject
	{
	public:
		/// <summary>
		/// ������ ������� �� �����
		/// </summary>
		/// <param name="window"></param>
		virtual void draw(const Window& window) = 0;

		/// <summary>
		/// ���������� ������ �������
		/// </summary>
		virtual void update() = 0;
	};

} // CW