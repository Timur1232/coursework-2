#pragma once

#include "pch.h"

#include "IDrawable.h"
#include "IUpdate.h"
#include "Events/Event.h"
#include "Events/EventInterface.h"
#include "Layer.h"

namespace CW {

	using namespace std::chrono_literals;

	class Application
		: public IDrawable,
		  public IUpdate,
		  public IOnEvent
	{
	public:
		Application() = delete;
		Application(int width, int height, const char* title);
		virtual ~Application() = default;

		virtual void UpdateInterface() = 0;
		virtual void PauseUpdate(float deltaTime) = 0;

		void UpdateLayers(float deltaTime);
		bool OnEventLayers(Event& event);

		sf::Vector2u GetWindowSize() const;
		const char* GetTitle() const;
		sf::Color GetClearColor() const { return m_ClearColor; }

		bool IsRunning() const;
		void Close();

		bool IsPaused() const;
		void SwitchPause();

		template <std::derived_from<Layer> T, class... _Args>
		void PushLayer(_Args&&... args)
		{
			m_Layers.emplace_back(CreateUnique<Layer>(std::forward<_Args>(args)...));
		}

	protected:
		std::vector<Unique<Layer>> m_Layers;

		sf::Vector2u m_WindowSize;
		const char* m_WindowTitle;
		sf::Color m_ClearColor = sf::Color::Black;

		bool m_Running = true;
		bool m_Pause = false;
	};

} // CW