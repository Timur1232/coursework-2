#pragma once

#include <Window.h>

namespace CW
{

	class ISimulationObject
	{
	public:
		/// <summary>
		/// Рендер объекта на экран
		/// </summary>
		/// <param name="window"></param>
		virtual void draw(const Window& window) = 0;

		/// <summary>
		/// Обновление логики объекта
		/// </summary>
		virtual void update() = 0;
	};

} // CW