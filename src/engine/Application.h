#pragma once

#include <SFML/Graphics.hpp>

#include "Events.h"
#include "IDrawable.h"
#include "IUpdate.h"

namespace CW_E {

	class Application
		: public IDrawable,
		  public IUpdate
	{
	public:
		Application() = delete;
		Application(int width, int height, const char* title,
			EventHandlerWrapper eventHandler,
			UpdateHandlerWrapper updateHandler);
		virtual ~Application() = default;

		sf::Vector2u getWindowSize() const;
		const char* getTitle() const;

		bool isRunning() const;
		void close();

		EventHandlerWrapper getEventHandler() const;
		UpdateHandlerWrapper getUpdateHandler() const;

	private:
		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;

		bool m_Running = true;

		EventHandlerWrapper m_EventHandler;
		UpdateHandlerWrapper m_UpdateHandler;
	};

} // CW_E