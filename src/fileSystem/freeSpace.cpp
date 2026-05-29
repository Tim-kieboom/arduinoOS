#include "mod.hpp"
#include <EEPROM.h>

static void swap(fileSystem::EntryAddress arr[10], u8 i1, u8 i2) {
    auto temp = arr[i1];
    arr[i1] = arr[i2];
    arr[i2] = temp;   
}

namespace fileSystem {
    constexpr u16 dataStart = FAT_Header::SIZE + FAT_Header::MAX_NUM_FILES * FATEntry::SIZEOF + 1;

    inline void freeSpace_begin(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    inline void freeSpace_getGap(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    inline void freeSpace_bubbleSortEntrys(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    inline void freeSpace_collectAddresses(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);

    Task freeSpace(MutRef<FreeSpaceState> stateRef, MutRef<int> addressRef, u16 size) {
        auto& state = stateRef.ref;
        auto& address = addressRef.ref;

        switch(state.taskId) {
            case FreeSpaceState::Begin:
                freeSpace_begin(out(state), out(address), size);    
                return Task::Pending();
            
            case FreeSpaceState::CollectAddresses:
                freeSpace_collectAddresses(out(state), out(address), size);
                return Task::Pending();
        
            case FreeSpaceState::BubbleSortEntrys:
                freeSpace_bubbleSortEntrys(out(state), out(address), size);
                return Task::Pending();
            
            case FreeSpaceState::GetGap:
                freeSpace_getGap(out(state), out(address), size);
                return Task::Pending();

            case FreeSpaceState::End:
                return Task::Done();

            default: DEBUG_UNREACHABLE;
        }

        return Task::Done();
    }

    inline void freeSpace_begin(MutRef<FreeSpaceState> stateRef, MutRef<int> address, u16 size) {
        auto& state = stateRef.ref;
        auto enoughSpace = [&]() {return EEPROM.length() - dataStart >= size;};

        state.numFiles = FAT::numFiles();
        if(state.numFiles == 0) {
            address.ref = enoughSpace() ? dataStart : ADDRESS_ERROR;
            state.taskId = FreeSpaceState::End;
            return;
        }

        state.taskId = FreeSpaceState::CollectAddresses;
    }

    inline void freeSpace_collectAddresses(MutRef<FreeSpaceState> stateRef, MutRef<int> address, u16 size) {
        auto& state = stateRef.ref;
        auto endLoop = [&](){return state.i >= state.numFiles;};

        if(endLoop()) {
            state.i = 0;
            state.taskId = FreeSpaceState::BubbleSortEntrys;
            return;
        }
        
        FATEntry entry;
        if(!FAT::entry(state.i, out(entry))) {
            DEBUG_PRINTM(F("!!error!! in freeSpace_collectAddresses() entry "), state.i, F(" not found\n"));
            address.ref = ADDRESS_ERROR;
            state.taskId = FreeSpaceState::End;
            return;
        }

        state.addressAt(state.i).start = entry.address;
        state.addressAt(state.i).end = entry.address + entry.size;

        state.i++;
        return;
    }

    inline void freeSpace_bubbleSortEntrys(MutRef<FreeSpaceState> stateRef, MutRef<int> address, u16 size) {
        auto& state = stateRef.ref;
        auto& i = state.i;
        auto& j = state.j;

        /*
        non async version would be:
        ```cpp

            for(u8 i = 0; i < state.numFiles - 1; i++) { //Loop

                for(u8 j = 0; j < state.numFiles - 1 - i; j++) { //Innerloop

                    if(state.addressAt(j).start > state.addressAt(j+1).start)
                        swap(state.addresses, j, j+1);
                }
            }

            if(state.addressAt(0).start - dataStart >= size) {
                address.ref = dataStart;
                state.taskId = FreeSpaceState::End;
                return Task::Done();
            }

            state.taskId = FreeSpaceState::GetGap;
            return Task::Pending();
        ```
        */

        auto endLoop = [&]() {return i >= state.numFiles-1;};
        auto endInnerLoop = [&]() {return j >= state.numFiles - 1 - i;};

        if(endInnerLoop()) {
            j = 0;
            i++;
        }

        if(endLoop()) {
            
            if(state.addressAt(0).start - dataStart >= size) {
                address.ref = dataStart;
                state.taskId = FreeSpaceState::End;
                return;
            }
            
            i = 0;
            j = 0;
            state.taskId = FreeSpaceState::GetGap;
            return;
        }

        if(state.addressAt(j).start > state.addressAt(j+1).start)
            swap(state.addresses, j, j+1); 

        j++;
    }

    inline void freeSpace_getGap(MutRef<FreeSpaceState> stateRef, MutRef<int> address, u16 size) {
        auto& state = stateRef.ref;
        auto& i = state.i;
        auto endLoop = [&]() {return i >= state.numFiles-1;};

        if(endLoop()) {
            u16 tailGap = EEPROM.length() - state.addressAt(state.numFiles - 1).end;
            
            if(tailGap >= size)
                address.ref = state.addressAt(state.numFiles - 1).end;
            else 
                address.ref = ADDRESS_ERROR;
            
            state.taskId = FreeSpaceState::End;
            return;
        }

        u16 gap = state.addressAt(i+1).start - state.addressAt(i).end;
        if(gap >= size) {
            address.ref = state.addressAt(i).end;
            state.taskId = FreeSpaceState::End;
            return; 
        }
        i++;
    }
}