#include "Window.h"

#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "Events.h"

namespace CW
{

	Window::Window(int width, int height, const char* title)
	{
		createWindow(width, height, title);
	}

	Window::~Window()
	{
		destroyWindow();
		glfwTerminate();
	}

	int Window::createWindow(int width, int height, const char* title)
	{
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return -1;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		// Создание окна
		m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!m_Window)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(m_Window);

		// Инициализация GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Failed to initialize GLEW" << std::endl;
			glfwTerminate();
			return -1;
		}

		glViewport(0, 0, width, height);
		m_Width = width;
		m_Height = height;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_Events.bindWindow(m_Window);

		return 0;
	}

	void Window::destroyWindow()
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
		glfwMakeContextCurrent(nullptr);
	}

	bool Window::operator!() const
	{
		return !m_Window;
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Window::setShouldClose(bool flag)
	{
		glfwSetWindowShouldClose(m_Window, flag);
	}

	void Window::swapBuffers() const
	{
		glfwSwapBuffers(m_Window);
	}

	int Window::width() const
	{
		return m_Width;
	}

	int Window::height() const
	{
		return m_Height;
	}

	glm::ivec2 Window::resolution() const
	{
		return glm::ivec2(m_Width, m_Height);
	}

	const GLFWwindow* Window::getWindowPtr() const
	{
		return m_Window;
	}

	const Events& Window::events() const
	{
		return m_Events;
	}

	void Window::clearScreen() const
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

} // CW