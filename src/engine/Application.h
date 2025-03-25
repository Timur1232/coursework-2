#pragma once

#include <SFML/Graphics.hpp>

#include "Events.h"
#include "IDrawable.h"

namespace CW {

	class Application
		: public IOnEvent,
		  public IDrawable
	{
	public:
		Application() = delete;
		explicit Application(int width, int height, const char* title);

		virtual void update() = 0;

		sf::Vector2u getWindowSize() const { return m_WindowSize; }
		const char* getTitle() const { return m_WindowTitle; }

		bool isRunning() const { return m_Running; }
		void close() { m_Running = false; }

	private:
		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;

		bool m_Running = true;
	};

} // CW