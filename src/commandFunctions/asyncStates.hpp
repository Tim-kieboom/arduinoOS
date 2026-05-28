#pragma once
#include "mod.hpp"
#include "../fileSystem/mod.hpp"

namespace commandFunctions {
    /// Async state holder for the `store` command, persists across loop cycles.
    struct StoreState {
        bool first = true;                                              ///< true on the first call, false after arguments are parsed.
        fileSystem::FileInfo file = fileSystem::FileInfo();             ///< Parsed file info (name, data, size).
        fileSystem::StoreState storeState = fileSystem::StoreState();   ///< Underlying file-system store state.
    };
}