#pragma once

#include <glm/glm.hpp>
#include <Window.h>

namespace CW
{
	
	constexpr float MIN_ZOOM = 0.2f;
	constexpr float MAX_ZOOM = 10.0f;
	constexpr float ZOOM_STEP = 0.2f;

	class Camera2D
	{
	public:
		Camera2D() = default;
		Camera2D(glm::vec2 position, float zoom = 1.0f);

		void zoomIn();
		void zoomOut();
		void update(const Window& window);

		float zoom() const;

	public:
		glm::vec2 position = { 0.0f, 0.0f };

	private:
		float m_Zoom = 1.0f;
	};

} // CW