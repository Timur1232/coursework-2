#pragma once

#include "pch.h"

#include "IDrawable.h"
#include "IUpdate.h"
#include "Events/Event.h"
#include "Events/EventInterface.h"
#include "Layer.h"
#include "debug_utils/Log.h"

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

		virtual void PauseUpdate(float deltaTime) = 0;

		void UpdateLayers(float deltaTime);
		bool OnEventLayers(Event& event);
		void DrawLayers();

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
			m_Layers.emplace_back(CreateUnique<T>(std::forward<_Args>(args)...));
		}

		template <std::derived_from<Layer> T, class... _Args>
		void InsertLayer(size_t index, _Args&&... args)
		{
			if (index > m_Layers.size())
				CW_CRITICAL("Layer index out of bounds when inserting! \nindex = {}\nm_Layers.size() = {}", index, m_Layers.size());
			m_Layers.emplace(m_Layers.begin() + index, CreateUnique<T>(std::forward<_Args>(args)...));
		}

		void PopLayer() { m_Layers.pop_back(); }
		void EraseLayer(size_t index)
		{
			if (index >= m_Layers.size())
				CW_CRITICAL("Layer index out of bounds when erasing! \nindex = {}\nm_Layers.size() = {}", index, m_Layers.size());
			m_Layers.erase(m_Layers.begin() + index);
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