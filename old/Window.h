#pragma once

#include "Events.h"

struct GLFWwindow;

namespace CW
{

	class Window
	{
	public:
		Window() = delete;
		Window(int width, int height, const char* title);
		~Window();

		int createWindow(int width, int height, const char* title);
		void destroyWindow();

		bool operator!() const;

		bool shouldClose() const;
		int width() const;
		int height() const;
		glm::ivec2 resolution() const;
		const GLFWwindow* getWindowPtr() const;

		const Events& events() const;

		void setShouldClose(bool flag);
		void swapBuffers() const;
		void clearScreen() const;

	private:
		GLFWwindow* m_Window = nullptr;
		int m_Width = 0;
		int m_Height = 0;
		Events m_Events;
	};

} // CW
