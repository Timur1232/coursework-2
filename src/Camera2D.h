#pragma once

#include <SFML/Graphics.hpp>

#include "engine/Events.h"

#include "engine/IUpdate.h"
#include "engine/IDrawable.h"

class Camera2D
	: public CW::OnMouseButtonPressed,
	  public CW::OnMouseButtonReleased,
	  public CW::OnMouseMoved,
	  public CW::OnMouseWheelScrolled,
	  public CW::IUpdate
{
public:
	Camera2D(float x, float y, float width, float height,
		CW::EventHandlerWrapper eventHandler,
		CW::UpdateHandlerWrapper updateHandler);

	void update() override;

	void onMouseButtonPressed(sf::Event::MouseButtonPressed e) override;
	void onMouseButtonReleased(sf::Event::MouseButtonReleased e) override;
	void onMouseMoved(sf::Event::MouseMoved e) override;
	void onMouseWheelScrolled(sf::Event::MouseWheelScrolled e) override;

	const sf::View& getView() const;

private:
	sf::View m_View;
	sf::Vector2i m_PrevPos{};
	float m_ZoomFactor = 1.0f;
	bool m_IsMoving = false;
};

