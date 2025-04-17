#pragma once

#include "pch.h"

#include "Events.h"
#include "IDrawable.h"
#include "IUpdate.h"

namespace CW {

	class Application
		: public IDrawable,
		  public IUpdate,
		  public virtual OnEvent
	{
	public:
		Application() = delete;
		Application(int width, int height, const char* title);
		virtual ~Application() = default;

		sf::Vector2u getWindowSize() const;
		const char* getTitle() const;

		bool isRunning() const;
		void close();

	protected:
		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;

		bool m_Running = true;
	};

} // CW