#pragma once

#include "pch.h"

#include "engine/Events.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"

namespace CW {

	class Camera2D
		: public MouseButtonPressedObs,
		  public MouseButtonReleasedObs,
		  public MouseMovedObs,
		  public MouseWheelScrolledObs,
		  public ResizedObs,
		  public IUpdate
	{
	public:
		Camera2D(float x, float y, float width, float height);

		void DebugInterface();

		void Update(sf::Time deltaTime) override;

		void OnMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override;
		void OnMouseButtonReleased(const sf::Event::MouseButtonReleased* e) override;
		void OnMouseMoved(const sf::Event::MouseMoved* e) override;
		void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled* e) override;

		void OnResized(const sf::Event::Resized* e) override;

		const sf::View& GetView() const;

		sf::Vector2f WorldPosition(sf::Vector2i mousePos) const;

	private:
		sf::View m_View;
		sf::Vector2i m_PrevPos{};
		float m_ZoomFactor = 1.0f;
		bool m_IsMoving = false;
	};

} // CW