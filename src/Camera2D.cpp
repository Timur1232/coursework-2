#include "pch.h"
#include "Camera2D.h"

#include "debug_utils/Log.h"

namespace CW {

	Camera2D::Camera2D(float x, float y, float width, float height)
		: m_View({ x, y }, { width, height })
	{
	}

	void Camera2D::DebugInterface()
	{
		ImGui::Text("camera position: (%.2f, %.2f)", m_View.getCenter().x, m_View.getCenter().y);
		ImGui::Text("camera zoom factor: %.2f", m_ZoomFactor);
		ImGui::Text("camera is moving: %d", m_IsMoving);
		ImGui::Text("mouse position: (%d, %d)", m_PrevPos.x, m_PrevPos.y);
		ImGui::Text("mouse world position: (%f, %f)", WorldPosition(m_PrevPos).x, WorldPosition(m_PrevPos).y);
	}

	void Camera2D::Update(sf::Time deltaTime)
	{
		
	}

	void Camera2D::OnMouseButtonPressed(const sf::Event::MouseButtonPressed* e)
	{
		if (!ImGui::GetIO().WantCaptureMouse)
			m_IsMoving = e->button == sf::Mouse::Button::Right;
	}

	void Camera2D::OnMouseButtonReleased(const sf::Event::MouseButtonReleased* e)
	{
		m_IsMoving = m_IsMoving && e->button != sf::Mouse::Button::Right;
	}

	void Camera2D::OnMouseMoved(const sf::Event::MouseMoved* e)
	{
		if (m_IsMoving)
		{
			m_View.move((sf::Vector2f)(m_PrevPos - e->position) * m_ZoomFactor);
		}
		m_PrevPos = e->position;
	}

	void Camera2D::OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled* e)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return;
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

	void Camera2D::OnResized(const sf::Event::Resized* e)
	{
		m_View.setSize((sf::Vector2f)e->size);
		m_View.zoom(m_ZoomFactor);
	}

	const sf::View& Camera2D::GetView() const
	{
		return m_View;
	}

	const float Camera2D::GetZoomFactor() const
	{
		return m_ZoomFactor;
	}

	sf::Vector2f Camera2D::WorldPosition(sf::Vector2i mousePos) const
	{
		return sf::Vector2f(
			m_View.getCenter().x - m_View.getSize().x / 2 + mousePos.x * m_ZoomFactor,
			m_View.getCenter().y - m_View.getSize().y / 2 + mousePos.y * m_ZoomFactor
		);
	}

} // CW