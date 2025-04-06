#pragma once

#include <SFML/Graphics.hpp>

#include "engine/Events.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"

namespace CW {

	class Camera2D
		: public CW_E::OnMouseButtonPressed,
		  public CW_E::OnMouseButtonReleased,
		  public CW_E::OnMouseMoved,
		  public CW_E::OnMouseWheelScrolled,
		  public CW_E::OnResized,
		  public CW_E::IUpdate
	{
	public:
		Camera2D(float x, float y, float width, float height);

		void update(sf::Time deltaTime) override;

		void onMouseButtonPressed(const sf::Event::MouseButtonPressed* e) override;
		void onMouseButtonReleased(const sf::Event::MouseButtonReleased* e) override;
		void onMouseMoved(const sf::Event::MouseMoved* e) override;
		void onMouseWheelScrolled(const sf::Event::MouseWheelScrolled* e) override;

		void onResized(const sf::Event::Resized* e) override;

		const sf::View& getView() const;

		sf::Vector2f worldPosition(sf::Vector2i mousePos) const;

	private:
		sf::View m_View;
		sf::Vector2i m_PrevPos{};
		float m_ZoomFactor = 1.0f;
		bool m_IsMoving = false;
	};

} // CW