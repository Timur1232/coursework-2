#pragma once

#include <memory>

#include "ProgramCore.h"

std::unique_ptr<CW::Application> create_program(int argc, const char** argv);

int main(int argc, const char** argv)
{
	std::unique_ptr<CW::ProgramCore> program = std::make_unique<CW::ProgramCore>();
	program->subscribeOnEvents();
	program->setApplication(create_program(argc, argv));
	program->run();
}