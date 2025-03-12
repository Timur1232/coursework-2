#pragma once

#include <glm/glm.hpp>

namespace Coursework
{
	
	constexpr float m_MinZoom = 0.25f;

	class Camera2D
	{
	public:
		Camera2D() = default;
		Camera2D(glm::vec2 position, float zoom = 1.0f);

		void zoomIn();
		void zoomOut();

	public:
		glm::vec2 position = { 0.0f, 0.0f };

	private:
		float m_Zoom = 1.0f;
	};

} // Coursework