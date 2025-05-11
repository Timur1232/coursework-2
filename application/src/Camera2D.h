#pragma once

#include "pch.h"

#include "engine/Events.h"
#include "engine/EventInterface.h"
#include "engine/CoreEvents.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"

namespace CW {

	class Camera2D
		: public IOnEvent
	{
	public:
		Camera2D(float x, float y, float width, float height);

		void DebugInterface();

		bool OnEvent(Event& event) override;

		const sf::View& GetView() const;
		const float GetZoomFactor() const;

		sf::Vector2f PixelToWorldPosition(sf::Vector2i mousePos) const;
		
	private:
		bool onMouseButtonPressed(MouseButtonPressed& e);
		bool onMouseButtonReleased(MouseButtonReleased& e);
		bool onMouseMoved(MouseMoved& e);
		bool onMouseWheelScrolled(MouseWheelScrolled& e);
		bool onResized(WindowResized& e);

	private:
		sf::View m_View;
		sf::Vector2i m_PrevPos{};
		float m_ZoomFactor = 1.0f;
		bool m_IsMoving = false;
	};

} // CW