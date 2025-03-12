#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Coursework
{

	class Shader
	{
	public:
		Shader() = default;
		~Shader();

		void loadShader(const char* vertexShader, const char* fragmentShader);
		void deleteShader();
		bool loaded();

		void use();

		void setUniform(const char* name, int data);
		void setUniform(const char* name, float data);
		void setUniform(const char* name, const glm::vec2& data);
		void setUniform(const char* name, const glm::vec3& data);
		void setUniform(const char* name, const glm::vec4& data);
		void setUniform(const char* name, const glm::mat4& data);

	private:
		uint32_t m_Id = 0;
	};

} // Coursework