#pragma once

#include <glm/glm.hpp>

#include "Shader.h"


namespace CW
{

	class Window;

	class Render2D
	{
	public:
		Render2D() = default;

		int initialize();

		//void drawCircle(const glm::vec2& pos, float radius, float thickness, const glm::vec3& color, const Window& window);
		void drawBox(const glm::vec2& pos, float width, float height, float thickness, const glm::vec3& color, const Window& window);
		//void drawLine(const glm::vec2& pos1, const glm::vec2& pos2, const Window& window);

	public:
		static const float SCREEN_SPACE_QUAD[8];
		static const uint32_t SCREEN_SPACE_INDECIES[6];

	private:
		int loadBoxShader();
		void bindAll();
		void unbindAll();

	private:
		static uint32_t sm_VAO;
		static uint32_t sm_VBO;
		static uint32_t sm_EBO;

		static Shader sm_BoxShader;
	};

} // CW