#include "Events.h"

#include <GLFW\glfw3.h>
#include <string.h>
#include <iostream>

#include <Window.h>

namespace CW
{

	bool Events::sm_Keys[KEYS_SIZE] = { 0 };
	uint64_t Events::sm_Frames[KEYS_SIZE] = { 0 };
	uint64_t Events::sm_CurrentFrame = 0;

	glm::dvec2 Events::sm_MousePos = glm::vec2(0.0, 0.0);
	glm::dvec2 Events::sm_MouseDeltaPos = glm::vec2(0.0, 0.0);
	double Events::sm_MouseScrollDelta = 0.0;

	bool Events::sm_CursorLoked = false;
	bool Events::sm_CursorStarted = false;
	bool Events::sm_MouseRelease = false;

	int Events::bindWindow(GLFWwindow* window)
	{
		if (!window) return -1;
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetCursorPosCallback(window, cursorPositionCallback);
		glfwSetScrollCallback(window, scrollCallback);
		return 0;
	}

	void Events::pollEvents() const
	{
		sm_CurrentFrame++;
		sm_MouseDeltaPos = glm::vec2(0.0);
		sm_MouseScrollDelta = 0.0;
		glfwPollEvents();
	}

	bool Events::keyPressed(int keycode) const
	{
		if (keycode < 0 || keycode >= MOUSE_BUTTONS)
			return false;
		return sm_Keys[keycode];
	}

	bool Events::keyJPressed(int keycode) const
	{
		if (keycode < 0 || keycode >= MOUSE_BUTTONS)
			return false;
		return sm_Keys[keycode] && sm_Frames[keycode] == sm_CurrentFrame;
	}

	bool Events::mousePressed(int button) const
	{
		int index = MOUSE_BUTTONS + button;
		return sm_Keys[index];
	}

	bool Events::mouseReleased(int button) const
	{
		int index = MOUSE_BUTTONS + button;
		return !sm_Keys[index] && (sm_Frames[index] == sm_CurrentFrame);
	}

	bool Events::mouseJPressed(int button) const
	{
		int index = MOUSE_BUTTONS + button;
		return sm_Keys[index] && (sm_Frames[index] == sm_CurrentFrame);
	}

	const glm::dvec2& Events::mousePos() const
	{
		return sm_MousePos;
	}

	const glm::dvec2& Events::mouseDeltaPos() const
	{
		return sm_MouseDeltaPos;
	}

	double Events::mouseScrollDelta() const
	{
		return sm_MouseScrollDelta;
	}

	void Events::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (action == GLFW_PRESS)
		{
			sm_Keys[key] = true;
			sm_Frames[key] = sm_CurrentFrame;
		}
		else if (action == GLFW_RELEASE)
		{
			sm_Keys[key] = false;
			sm_Frames[key] = sm_CurrentFrame;
		}
	}

	void Events::mouseButtonCallback(GLFWwindow* window, int button, int action, int mode)
	{
		int index = MOUSE_BUTTONS + button;

		if (action == GLFW_PRESS)
		{
			sm_Keys[index] = true;
			sm_Frames[index] = sm_CurrentFrame;
		}
		else if (action == GLFW_RELEASE)
		{
			sm_Keys[index] = false;
			sm_Frames[index] = sm_CurrentFrame;
		}
	}

	void Events::cursorPositionCallback(GLFWwindow* window, double x_pos, double y_pos)
	{
		if (sm_CursorStarted)
		{
			sm_MouseDeltaPos.x += x_pos - sm_MousePos.x;
			sm_MouseDeltaPos.y += y_pos - sm_MousePos.y;
		}
		else
		{
			sm_CursorStarted = true;
		}
		sm_MousePos.x = x_pos;
		sm_MousePos.y = y_pos;
	}

	void Events::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		sm_MouseScrollDelta = yoffset;
	}

} // CW