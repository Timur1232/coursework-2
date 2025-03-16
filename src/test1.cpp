#include "GL.h"

#include <iostream>

#include <Window.h>
#include <Shader.h>
#include <Camera2D.h>

using namespace CW;

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

	Camera2D camera;

	auto clamp = [](float a, float min, float max) -> float {
		if (a < min) a = min;
		else if (a > max) a = max;
		return a;
	};

    while (!window.shouldClose())
    {
		window.clearScreen();

		camera.update(window);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        shader.use();
        shader.setUniform("uCameraPos", camera.position);
		shader.setUniform("uCameraZoom", camera.zoom());
		shader.setUniform("uResolution", window.resolution());
        glDrawArrays(GL_TRIANGLES, 0, 6);

        window.swapBuffers();
        window.events().pollEvents();
    }
}