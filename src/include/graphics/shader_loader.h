#pragma once

#include <string>

namespace CW
{

	[[nodiscard]]
	uint32_t compile_shader(const char* shaderPath, uint32_t type);
	
	[[nodiscard]]
	uint32_t link_program(const uint32_t* shaderIDs, size_t num);

} // CW