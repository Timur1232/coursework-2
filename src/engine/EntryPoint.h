#pragma once

#include <memory>

#include "ProgramCore.h"

std::unique_ptr<CW_E::Application> create_program(int, const char**, CW_E::EventHandlerWrapper, CW_E::UpdateHandlerWrapper);

int main(int argc, const char** argv)
{
	std::unique_ptr<CW_E::ProgramCore> program = std::make_unique<CW_E::ProgramCore>();
	program->setApplication(create_program(argc, argv, program->getEventHandler(), program->getUpdateHandler()));
	program->run();
}