#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "window/Window.h"
#include "graphics/Shader.h"
#include "graphics/loaders/shader_loader.h"

using namespace Coursework;

struct alignas(8) Agent
{
    glm::vec2 position;
    glm::vec2 velosity{0.0f};
    float mass = 100.0f;
    float radius;

    Agent()
        : velosity(0.0f), mass(100.0f)
    {
        position = glm::vec2(rand() % 480 + 10, rand() % 480 + 10);
        radius = rand() % (5 - 2) + 2;
        mass += radius * 30.0f;
    }

    Agent(glm::vec2 pos, glm::vec2 vel, float mass, float radius)
        : position(pos), velosity(vel), mass(mass), radius(radius)
    {
    }
};

enum BufferBase
{
    INPUT_BUFFER = 0,
    OUTPUT_BUFFER = 1
};

//std::chrono::steady_clock::time_point deltaTime()
//{
//    static auto current = std::chrono::high_resolution_clock::now();
//    current = std::chrono::high_resolution_clock::now() - current;
//    return current = std::chrono::high_resolution_clock::now() - current;
//}

int main()
{
	Window window(500, 500, "Window 1");
	if (!window)
	{
		std::cerr << "Failed to create window" << std::endl;
		return -1;
	}

    srand(time(0));

    float vertices[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1, -1,
        -1,  1,
         1,  1
    };

    Shader agentShader;
    agentShader.loadShader("res/shaders/vertex/v_full_window.glsl", "res/shaders/fragment/f_agent.glsl");
    Shader gridShader;
    gridShader.loadShader("res/shaders/vertex/v_full_window.glsl", "res/shaders/fragment/f_grid.glsl");

    uint32_t computeShader;
    {
        uint32_t shaderTmp = compile_shader("res/shaders/compute/c_agent.glsl", GL_COMPUTE_SHADER);
        computeShader = link_program(&shaderTmp, 1);
    }

    std::cout << "Size of Agent: " << sizeof(Agent) << std::endl;

    uint32_t dataBuffer[2];
    glGenBuffers(2, dataBuffer);
    std::vector<Agent> data = { {{250.0f, 250.0f}, {0, 0}, 1000.0f, 6.0f}, {{200.0f, 250.0f}, {0, -3.5f}, 50.0f, 2.0f} };

    for (auto& i : data)
    {
        std::cout << "x: " << i.position.x << ", y: " << i.position.y << std::endl;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Agent), data.data(), GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Agent), data.data(), GL_DYNAMIC_COPY);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float shiftY = 0.0f;

    while (!window.shouldClose())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INPUT_BUFFER, dataBuffer[INPUT_BUFFER]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OUTPUT_BUFFER, dataBuffer[OUTPUT_BUFFER]);

        glUseProgram(computeShader);
        glUniform1i(glGetUniformLocation(computeShader, "uAgentsSize"), data.size());
        glUniform1f(glGetUniformLocation(computeShader, "uG_const"), 0.5f);
        glUniform1f(glGetUniformLocation(computeShader, "uDeltaTime"), 0.016f);
        glUniform2f(glGetUniformLocation(computeShader, "uResolution"), window.getWidth(), window.getHeight());
        glDispatchCompute(data.size(), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); 

        std::swap(dataBuffer[INPUT_BUFFER], dataBuffer[OUTPUT_BUFFER]);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        gridShader.use();
        gridShader.setUniform("uResolution", glm::vec2(window.getWidth(), window.getHeight()));
        gridShader.setUniform("uGridWidth", (int)(window.getWidth() / 10));
        gridShader.setUniform("uGridHeight", (int)(window.getHeight() / 10));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        agentShader.use();
        agentShader.setUniform("uAgentsSize", (int)data.size());
        agentShader.setUniform("uResolution", glm::vec2(window.getWidth(), window.getHeight()));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        window.swapBuffers();
        window.events().pollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(2, dataBuffer);

    return 0;

	std::cin.get();
}