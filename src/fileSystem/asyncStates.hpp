#pragma once 
#include "FAT/mod.hpp"
#include "fileStore/mod.hpp"

namespace fileSystem {
    struct EntryAddress {
        u16 start;
        u16 end;
    };
    
    struct FreeSpaceState: public IAsyncState {
        enum TaskId {
            Begin,
            CollectAddresses,
            BubbleSortEntrys,
            GetGap,
            End,
        };

        u8 i = 0;
        u8 j = 0;
        u8 numFiles = 0;
        TaskId taskId = Begin;
        EntryAddress addresses[FAT_Header::MAX_NUM_FILES];

        EntryAddress& addressAt(u8 i) {
            ASSERT(i < FAT_Header::MAX_NUM_FILES-1);
            return addresses[i];
        }

        void reset() {
            this->i = 0;
            this->j = 0;
            this->taskId = Begin;
        }
    };

    /// Async state holder for the file-store operation.
    struct StoreState: public IAsyncState {
        enum TaskId {
            CheckName,
            CheckForDuplicate,
            WriteFAT,
            WriteData,
            End,
        };
        
        u16 i = 0;                       ///< Loop counter
        TaskId taskId = CheckName;
        FATEntry fatEntry = FATEntry(); ///< The FAT entry being written.
        FreeSpaceState freeSpaceState = FreeSpaceState();
        int writeAddress = 0;            ///< Persistent address from freeSpace (survives async calls).

        void reset() {
            this->freeSpaceState.reset();
            this->taskId = CheckName;
            this->i = 0;
            this->writeAddress = 0;
        }
    };

    /// Async state holder for the print files operation.
    struct PrintFilesState {
        enum TaskId {
            CheckName,
            IndexesLoaded,
        };

        u8 i = 0;                          ///< Loop counter for indexes.
        TaskId taskId = CheckName;        
        u8 indexesLen = 0;                  ///< length of indexes array
        u8 indexes[fileStore::MAX_INDEXES]; ///< indexes of existing files in FAT

        bool first() {
            return taskId == CheckName;
        }

        void reset() {
            this->i = 0;
            this->indexesLen = 0;
            this->taskId = CheckName;
        }
    };
}