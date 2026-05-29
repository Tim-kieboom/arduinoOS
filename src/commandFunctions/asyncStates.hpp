#pragma once
#include "mod.hpp"
#include "../fileSystem/mod.hpp"

namespace commandFunctions {
    /// Async state holder for the `store` command, persists across loop cycles.
    struct StoreState {
        enum TaskId {
            CheckName,
            ArgumentsParsed,
        };

        TaskId taskId = CheckName;
        fileSystem::FileInfo file = fileSystem::FileInfo();             ///< Parsed file info (name, data, size).
        fileSystem::StoreState storeState = fileSystem::StoreState();   ///< Underlying file-system store state.

        void reset() {
            this->taskId = CheckName; 
            this->storeState.reset();
            this->file = fileSystem::FileInfo();
        }
    };
}