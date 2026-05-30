/*
Tim Kieboom 1025003
*/
#pragma once 
#include "FAT/mod.hpp"
#include "asyncStates.hpp"

namespace fileSystem {
    constexpr int FREESPACE_ERROR = -1;

    /// Describes a file to be stored (name, data, size).
    struct FileInfo {
        StrSlice name = StrSlice(); ///< File name.
        StrSlice data = StrSlice(); ///< File data.
    };
    
    void clearFAT();
    bool erase(int fileIndex);
    bool recieve(MutRef<int> fileIndex, MutRef<char[BUFFER_SIZE]> data, MutRef<u8> dataSize);

    Task printFiles(MutRef<PrintFilesState> state);
    Task findFileIndex(MutRef<int> fileIndex, const char* name);
    Task store(MutRef<StoreState> state, FileInfo const& info);
    Task findFreeSpace(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    Task largestFreeSpace(MutRef<FreeSpaceState> state, MutRef<u16> maxGap, MutRef<int> result);
}