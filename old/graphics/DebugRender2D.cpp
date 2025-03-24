#include "graphics/DebugRender2D.h"

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <graphics/Shader.h>
#include <Window.h>

namespace CW
{

	uint32_t DebugRender2D::sm_VAO = 0;
	uint32_t DebugRender2D::sm_VBO = 0;
	uint32_t DebugRender2D::sm_EBO = 0;

	 Shader DebugRender2D::sm_CircleShader;
	Shader DebugRender2D::sm_BoxShader;

	const float DebugRender2D::SCREEN_SPACE_QUAD[8] = {
		//   x	   y
		   -1.0f,-1.0f,
			1.0f,-1.0f,
		   -1.0f, 1.0f,
			1.0f, 1.0f
	};

	const uint32_t DebugRender2D::SCREEN_SPACE_INDECIES[6] = {
			0, 1, 2,
			1, 2, 3
	};

	int DebugRender2D::initialize()
	{
		/*if (!sm_BoxShader.loaded())
			if (loadBoxShader())
				return -1;*/

		if (!sm_CircleShader.loaded())
			if (loadCircleShader())
				return -1;

		bool justGenerated = false;
		if (!sm_VAO)
		{
			glGenVertexArrays(1, &sm_VAO);
			justGenerated = true;
		}
		if (!sm_VBO)
		{
			glGenBuffers(1, &sm_VBO);
			justGenerated = true;
		}
		if (!sm_EBO)
		{
			glGenBuffers(1, &sm_EBO);
			justGenerated = true;
		}

		if (justGenerated)
		{
			bindAll();
			glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_SPACE_QUAD), SCREEN_SPACE_QUAD, GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SCREEN_SPACE_INDECIES), SCREEN_SPACE_INDECIES, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
			glEnableVertexAttribArray(0);
			unbindAll();
		}

		return 0;
	}

	void DebugRender2D::drawCircle(const glm::vec2& pos, float radius, const glm::vec3& color, const Window& window)
	{
		sm_CircleShader.use();
		sm_CircleShader.setUniform("uPosition", pos);
		sm_CircleShader.setUniform("uRadius", radius);
		//sm_CircleShader.setUniform("uThickness", thickness);
		sm_CircleShader.setUniform("uResolution", window.resolution());
		sm_CircleShader.setUniform("uColor", color);
		//sm_CircleShader.setUniform("uFade", 0.005f);
	
		bindAll();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		unbindAll();
	}

	void DebugRender2D::drawBox(const glm::vec2& pos, float width, float height, float thickness, const glm::vec3& color, const Window& window)
	{
		sm_BoxShader.use();
		sm_BoxShader.setUniform("iPosition", pos);
		sm_BoxShader.setUniform("iSize", glm::vec2(width, height));
		//sm_BoxShader.setUniform("iThickness", thickness);
		sm_BoxShader.setUniform("iResolution", window.resolution());
		sm_BoxShader.setUniform("iColor", color);
		//sm_BoxShader.setUniform("iFade", 0.005f);

		bindAll();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		unbindAll();
	}

	//void Render2D::drawLine(const glm::vec2& pos1, const glm::vec2& pos2, const Window& window)
	//{
	//}

	int DebugRender2D::loadCircleShader()
	{
		try
		{
			sm_CircleShader.loadShader("res/shaders/vertex/v_circle.glsl", "res/shaders/fragment/f_circle.glsl");
		}
		catch (std::runtime_error& err)
		{
			std::cerr << err.what() << std::endl;
			return -1;
		}
		return 0;
	}

	int DebugRender2D::loadBoxShader()
	{
		try
		{
			sm_BoxShader.loadShader("res/shaders/vertex/v_box.glsl", "res/shaders/fragment/f_box.glsl");
		}
		catch (std::runtime_error& err)
		{
			std::cerr << err.what() << std::endl;
			return -1;
		}
		return 0;
	}

	void DebugRender2D::bindAll()
	{
		glBindVertexArray(sm_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, sm_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sm_EBO);
	}

	void DebugRender2D::unbindAll()
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

} // CW
