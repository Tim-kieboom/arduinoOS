/*
Tim Kieboom 1025003
*/
#pragma once 
#include "FAT/mod.hpp"
#include "asyncStates.hpp"

namespace fileSystem {

    /// Describes a file to be stored (name, data, size).
    struct FileInfo {
        StrSlice name = StrSlice(); ///< File name.
        StrSlice data = StrSlice(); ///< File data.
    };
    
    /// Erases all files from the FAT and resets the system.
    void clearFAT();

    /// Stores a file asynchronously. Call repeatedly until the returned Task is done.
    Task store(MutRef<StoreState> state, FileInfo const& info);
    
    constexpr int FREESPACE_ERROR = -1;
    Task findFreeSpace(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    Task largestFreeSpace(MutRef<FreeSpaceState> state, MutRef<u16> maxGap, MutRef<int> result);
    /// prints all file names an sizes in FAT 
    Task printFiles(MutRef<PrintFilesState> state);
}