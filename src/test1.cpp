#include "GL.h"

#include <iostream>

#include "window/Window.h"
#include "graphics/Shader.h"

using namespace Coursework;

const float vertices[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		 1, -1,
		-1,  1,
		 1,  1
};

int main()
{
	Window window(800, 600, "Hui");
	if (!window)
	{
		std::cerr << "Unable to create window" << std::endl;
		return 1;
	}
	const glm::vec2 resolution = { window.getWidth(), window.getHeight() };

	Shader shader;
	shader.loadShader("res/shaders/vertex/v_shader.glsl", "res/shaders/fragment/f.glsl");

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glm::vec2 cameraPos = { 0.0f, 0.0f };
	float cameraZoom = 1.0f;

	auto clamp = [](float a, float min, float max) -> float {
		if (a < min) a = min;
		else if (a > max) a = max;
		return a;
	};

    while (!window.shouldClose())
    {
		window.clearScreen();

		if (window.events().keyJPressed(GLFW_KEY_UP))
		{
			cameraZoom = clamp(cameraZoom - 0.2f, 0.2f, 10.f);
			std::cout << "Zoom: " << cameraZoom << std::endl;
		}
		else if (window.events().keyJPressed(GLFW_KEY_DOWN))
		{
			cameraZoom = clamp(cameraZoom + 0.2f, 0.2f, 10.f);
			std::cout << "Zoom: " << cameraZoom << std::endl;
		}
		else if (window.events().mousePressed(GLFW_MOUSE_BUTTON_1))
		{
			glm::vec2 delta = window.events().mouseDeltaPos() / (glm::dvec2)resolution;
			delta *= 2.0f * cameraZoom;
			delta.y *= -1.0f;
			cameraPos -= delta;
		}
		else if (window.events().keyJPressed(GLFW_KEY_W))
		{
			cameraPos.y += 0.2f;
		}
		else if (window.events().keyJPressed(GLFW_KEY_S))
		{
			cameraPos.y -= 0.2f;
		}
		else if (window.events().keyJPressed(GLFW_KEY_D))
		{
			cameraPos.x += 0.2f;
		}
		else if (window.events().keyJPressed(GLFW_KEY_A))
		{
			cameraPos.x -= 0.2f;
		}

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        shader.use();
        shader.setUniform("uCameraPos", cameraPos);
		shader.setUniform("uCameraZoom", cameraZoom);
		shader.setUniform("uResolution", resolution);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        window.swapBuffers();
        window.events().pollEvents();
    }
}