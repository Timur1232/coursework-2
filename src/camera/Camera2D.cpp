#include "Camera2D.h"

#include <GL.h>
#include <utils/utils.h>
#include <Window.h>

namespace CW
{
	Camera2D::Camera2D(glm::vec2 position, float zoom)
		: position(position)
	{
		m_Zoom = clamp(zoom, MIN_ZOOM, MAX_ZOOM);
	}

	void Camera2D::zoomIn()
	{
		m_Zoom = clamp(m_Zoom - ZOOM_STEP, MIN_ZOOM, MAX_ZOOM);
	}

	void Camera2D::zoomOut()
	{
		m_Zoom = clamp(m_Zoom + ZOOM_STEP, MIN_ZOOM, MAX_ZOOM);
	}

	void Camera2D::update(const Window& window)
	{
		if (window.events().mousePressed(GLFW_MOUSE_BUTTON_1))
		{
			glm::vec2 delta = window.events().mouseDeltaPos() / (glm::dvec2)window.resolution();
			delta *= 2.0f * m_Zoom;
			delta.y *= -1.0f;
			position -= delta;
		}
		if (window.events().mouseScrollDelta() > 0)
		{
			zoomIn();
		}
		else if (window.events().mouseScrollDelta() < 0)
		{
			zoomOut();
		}
	}

	float Camera2D::zoom() const
	{
		return m_Zoom;
	}

} // CW