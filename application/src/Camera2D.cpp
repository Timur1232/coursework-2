#include "pch.h"
#include "Camera2D.h"

#include "debug_utils/Log.h"

namespace CW {

	Camera2D::Camera2D(float x, float y, float width, float height)
		: m_View({ x, y }, { width, height })
	{
	}

	void Camera2D::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		if (dispatcher.Dispach<MouseButtonPressed>(CW_BUILD_EVENT_FUNC(OnMouseButtonPressed)))
			return;
		if (dispatcher.Dispach<MouseButtonReleased>(CW_BUILD_EVENT_FUNC(OnMouseButtonReleased)))
			return;
		if (dispatcher.Dispach<MouseMoved>(CW_BUILD_EVENT_FUNC(OnMouseMoved)))
			return;
		if (dispatcher.Dispach<MouseWheelScrolled>(CW_BUILD_EVENT_FUNC(OnMouseWheelScrolled)))
			return;
		if (dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(OnResized)))
			return;
	}

	bool Camera2D::OnMouseButtonPressed(MouseButtonPressed& e)
	{
		if (!ImGui::GetIO().WantCaptureMouse)
			m_IsMoving = e.Data.button == sf::Mouse::Button::Right;
		return false;
	}

	bool Camera2D::OnMouseButtonReleased(MouseButtonReleased& e)
	{
		m_IsMoving = m_IsMoving && e.Data.button != sf::Mouse::Button::Right;
		return false;
	}

	bool Camera2D::OnMouseMoved(MouseMoved& e)
	{
		if (m_IsMoving)
		{
			m_View.move((sf::Vector2f)(m_PrevPos - e.Data.position) * m_ZoomFactor);
		}
		m_PrevPos = e.Data.position;
		return false;
	}

	bool Camera2D::OnMouseWheelScrolled(MouseWheelScrolled& e)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return false;
		if (e.Data.delta > 0 && m_ZoomFactor * 0.9f >= 0.25f)
		{
			m_View.zoom(0.9f);
			m_ZoomFactor *= 0.9f;
		}
		else if (e.Data.delta < 0 && m_ZoomFactor * 1.1f <= 10.0f)
		{
			m_View.zoom(1.1f);
			m_ZoomFactor *= 1.1f;
		}
		return false;
	}

	bool Camera2D::OnResized(WindowResized& e)
	{
		m_View.setSize((sf::Vector2f)e.Size);
		m_View.zoom(m_ZoomFactor);
		return false;
	}

	const sf::View& Camera2D::GetView() const
	{
		return m_View;
	}

	const float Camera2D::GetZoomFactor() const
	{
		return m_ZoomFactor;
	}

	sf::Vector2f Camera2D::PixelToWorldPosition(sf::Vector2i mousePos) const
	{
		return sf::Vector2f(
			m_View.getCenter().x - m_View.getSize().x / 2 + mousePos.x * m_ZoomFactor,
			m_View.getCenter().y - m_View.getSize().y / 2 + mousePos.y * m_ZoomFactor
		);
	}

} // CW