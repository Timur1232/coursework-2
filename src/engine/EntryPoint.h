#pragma once

#include <memory>

#include "ProgramCore.h"

std::unique_ptr<CW_E::Application> create_program(int argc, const char** argv);

int main(int argc, const char** argv)
{
	std::unique_ptr<CW_E::ProgramCore> program = std::make_unique<CW_E::ProgramCore>();
	program->subscribeOnEvents();
	program->setApplication(create_program(argc, argv));
	program->run();
}