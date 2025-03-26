#pragma once

#include <SFML/Graphics.hpp>

#include "Events.h"
#include "IDrawable.h"

namespace CW {

	class Application
		: public IDrawable
	{
	public:
		Application() = delete;
		explicit Application(int width, int height, const char* title);

		// ����� ��� ������������� ������ ���� ����� ������������� ������ ����.
		// ��������� ��� ������, ��������, getEventHandler()->subscribe(this).
		// ��������! ��� ������ ��������� ������� � �������� � ������������ ���������� ������,
		// ��� ��� ���� � ���� ������ ��� �� ������������������.
		virtual void coreInit() {}

		// ���� �������� ���������� ���������
		virtual void update() = 0;

		sf::Vector2u getWindowSize() const;
		const char* getTitle() const;

		bool isRunning() const;
		void close();

		void setEventHandler(EventHandlerWrapper handler);
		EventHandlerWrapper getEventHandler() const;

	private:
		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;

		bool m_Running = true;

		EventHandlerWrapper m_EventHandler;
	};

} // CW