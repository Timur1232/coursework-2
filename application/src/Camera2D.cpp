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
		ImGui::Text("mouse world position: (%f, %f)", PixelToWorldPosition(m_PrevPos).x, PixelToWorldPosition(m_PrevPos).y);
	}

	bool Camera2D::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispach<MouseButtonPressed>(CW_BUILD_EVENT_FUNC(onMouseButtonPressed));
		dispatcher.Dispach<MouseButtonReleased>(CW_BUILD_EVENT_FUNC(onMouseButtonReleased));
		dispatcher.Dispach<MouseMoved>(CW_BUILD_EVENT_FUNC(onMouseMoved));
		dispatcher.Dispach<MouseWheelScrolled>(CW_BUILD_EVENT_FUNC(onMouseWheelScrolled));
		dispatcher.Dispach<WindowResized>(CW_BUILD_EVENT_FUNC(onResized));
		return false;
	}

	bool Camera2D::onMouseButtonPressed(MouseButtonPressed& e)
	{
		if (!ImGui::GetIO().WantCaptureMouse)
			m_IsMoving = e.Data.button == sf::Mouse::Button::Right;
		return false;
	}

	bool Camera2D::onMouseButtonReleased(MouseButtonReleased& e)
	{
		m_IsMoving = m_IsMoving && e.Data.button != sf::Mouse::Button::Right;
		return false;
	}

	bool Camera2D::onMouseMoved(MouseMoved& e)
	{
		if (m_IsMoving)
		{
			m_View.move((sf::Vector2f)(m_PrevPos - e.Data.position) * m_ZoomFactor);
		}
		m_PrevPos = e.Data.position;
		return false;
	}

	bool Camera2D::onMouseWheelScrolled(MouseWheelScrolled& e)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return false;
		if (e.Data.delta > 0)
		{
			m_View.zoom(0.9f);
			m_ZoomFactor *= 0.9f;
		}
		else
		{
			m_View.zoom(1.1f);
			m_ZoomFactor *= 1.1f;
		}
		return false;
	}

	bool Camera2D::onResized(WindowResized& e)
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