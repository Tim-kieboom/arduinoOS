#pragma once
#include "mod.hpp"
#include "commandFunctions/mod.hpp"

namespace input {
    /**
     * @brief Reads a line of input from Serial and resolves it to a command.
     * @param inputRef  Mutable reference to the input buffer.
     * @param commandRef Mutable reference where the resolved command pointer is stored.
     * 
     * Reads tokens from Serial, looks up the first token as a command name,
     * and stores the matching CommandFunctionsPtr in commandRef.
     */
    void readCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFunctionsPtr> commandRef);
}