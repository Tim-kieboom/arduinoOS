#pragma once
#include "mod.hpp"
#include "../fileSystem/mod.hpp"

namespace commandFunctions {

    typedef fileSystem::PrintFilesState PrintFilesState;
    struct StoreState: public IAsyncState {
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

    struct FreeSpaceState: public IAsyncState {
        u16 maxGap = 0;
        int result = 0;
        fileSystem::FreeSpaceState state = fileSystem::FreeSpaceState();

        void reset() {
            this->maxGap = 0;
            this->result = 0;
            this->state.reset();
        }
    };

    struct FileFindState: public IAsyncState {
        const char* name = nullptr;
        int i = 0;

        void reset() {
            this->i = 0;
            this->name = nullptr;
        }
    };

    union AnyState {
        StoreState store;
        FileFindState fileFind;
        FreeSpaceState freeSpace;
        PrintFilesState printFiles;

        AnyState() {}
        ~AnyState() {}
    };
}