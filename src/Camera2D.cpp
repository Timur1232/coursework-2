#include "Camera2D.h"

#include "imgui.h"
#include "debug_utils/Log.h"

namespace CW {

	Camera2D::Camera2D(float x, float y, float width, float height,
		CW_E::EventHandlerWrapper eventHandler,
		CW_E::UpdateHandlerWrapper updateHandler)
		: m_View({ x, y }, { width, height })
	{
		eventHandler.subscribe(this);
		updateHandler.subscribe(this);
	}

	void Camera2D::update(sf::Time deltaTime)
	{
		ImGui::Begin("Debug");
		ImGui::Text("camera position: (%.2f, %.2f)", m_View.getCenter().x, m_View.getCenter().y);
		ImGui::Text("camera zoom factor: %.2f", m_ZoomFactor);
		ImGui::Text("camera is moving: %d", m_IsMoving);
		ImGui::Text("mouse position: (%d, %d)", m_PrevPos.x, m_PrevPos.y);
		ImGui::End();
	}

	void Camera2D::onMouseButtonPressed(const sf::Event::MouseButtonPressed* e)
	{
		m_IsMoving = e->button == sf::Mouse::Button::Right;
	}

	void Camera2D::onMouseButtonReleased(const sf::Event::MouseButtonReleased* e)
	{
		m_IsMoving = m_IsMoving && e->button != sf::Mouse::Button::Right;
	}

	void Camera2D::onMouseMoved(const sf::Event::MouseMoved* e)
	{
		if (m_IsMoving)
		{
			m_View.move((sf::Vector2f)(m_PrevPos - e->position) * m_ZoomFactor);
		}
		m_PrevPos = e->position;
	}

	void Camera2D::onMouseWheelScrolled(const sf::Event::MouseWheelScrolled* e)
	{
		if (e->delta > 0)
		{
			m_View.zoom(0.9f);
			m_ZoomFactor *= 0.9f;
		}
		else
		{
			m_View.zoom(1.1f);
			m_ZoomFactor *= 1.1f;
		}
	}

	void Camera2D::onResized(const sf::Event::Resized* e)
	{
		m_View.setSize((sf::Vector2f)e->size);
		m_View.zoom(m_ZoomFactor);
	}

	const sf::View& Camera2D::getView() const
	{
		return m_View;
	}

	sf::Vector2f Camera2D::worldPosition(sf::Vector2i mousePos) const
	{
		return sf::Vector2f(
			m_View.getCenter().x - m_View.getSize().x / 2 + mousePos.x * m_ZoomFactor,
			m_View.getCenter().y - m_View.getSize().y / 2 + mousePos.y * m_ZoomFactor
		);
	}

} // CW