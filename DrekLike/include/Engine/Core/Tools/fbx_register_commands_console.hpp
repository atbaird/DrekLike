#pragma once
#include "Engine/Console/Command.hpp"

#ifndef FBX_REGISTER_COMMANDS_CONSOLE_HPP
#define  FBX_REGISTER_COMMANDS_CONSOLE_HPP

void RegisterAllImportantFBXCommandsToConsole();

//Commands
void FbxList(Command& input);

#endif // !FBX_REGISTER_COMMANDS_CONSOLE_HPP
