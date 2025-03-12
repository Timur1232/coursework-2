#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

namespace Coursework
{

	class Window;

	constexpr int KEYS_SIZE = 1032;
	constexpr int MOUSE_BUTTONS = 1024;

	class Events
	{
	public:
		Events() = default;
		Events(const Events&) = delete;

		int bindWindow(GLFWwindow* window);
		void pollEvents() const;

		bool keyPressed(int keycode) const;
		bool keyJPressed(int keycode) const;

		bool mousePressed(int button) const;
		bool mouseJPressed(int button) const;
		bool mouseReleased(int button) const;

		const glm::dvec2& mousePos() const;
		const glm::dvec2& mouseDeltaPos() const;
		double mouseScrollDelta() const;

	private:
		static bool sm_Keys[KEYS_SIZE];
		static uint64_t sm_Frames[KEYS_SIZE];
		static uint64_t sm_CurrentFrame;

		static glm::dvec2 sm_MousePos;
		static glm::dvec2 sm_MouseDeltaPos;
		static double sm_MouseScrollDelta;

		static bool sm_CursorLoked;
		static bool sm_CursorStarted;
		static bool sm_MouseRelease;

	private:
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
		static void cursorPositionCallback(GLFWwindow* window, double x_pos, double y_pos);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	};

} // Coursework