#pragma once
#include "mod.hpp"
#include "../fileSystem/mod.hpp"

namespace commandFunctions {
    struct StoreState {
        bool first = true;
        fileSystem::FileInfo file = fileSystem::FileInfo();
        fileSystem::StoreState storeState = fileSystem::StoreState();
    };
}