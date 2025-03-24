#pragma once

#include <glm/glm.hpp>

#include "Shader.h"


namespace CW
{

	class Window;

	class DebugRender2D
	{
	public:
		DebugRender2D() = default;

		static int initialize();

		static void drawCircle(const glm::vec2& pos, float radius, const glm::vec3& color, const Window& window);
		static void drawBox(const glm::vec2& pos, float width, float height, float thickness, const glm::vec3& color, const Window& window);
		//void drawLine(const glm::vec2& pos1, const glm::vec2& pos2, const Window& window);

	public:
		static const float SCREEN_SPACE_QUAD[8];
		static const uint32_t SCREEN_SPACE_INDECIES[6];

	private:
		static int loadBoxShader();
		static int loadCircleShader();
		static void bindAll();
		static void unbindAll();

	private:
		static uint32_t sm_VAO;
		static uint32_t sm_VBO;
		static uint32_t sm_EBO;

		static Shader sm_BoxShader;
		static Shader sm_CircleShader;
	};

} // CW