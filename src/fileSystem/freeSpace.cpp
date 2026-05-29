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
    inline void findFreeSpace_getGap(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    inline void freeSpace_collectAddresses(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size);
    inline void largestFreeSpace_getGap(MutRef<FreeSpaceState> state, MutRef<u16> maxGap, MutRef<int> address);
    inline void freeSpace_bubbleSortEntrys(MutRef<FreeSpaceState> state, MutRef<int> address, u16 size, bool isFindFreeSpace);

    Task largestFreeSpace(MutRef<FreeSpaceState> stateRef, MutRef<u16> maxGapRef, MutRef<int> resultRef) {
        auto& state = stateRef.ref;
        auto& maxGap = maxGapRef.ref;
        auto& result = resultRef.ref;

        switch(state.taskId) {
            case FreeSpaceState::Begin:
                freeSpace_begin(out(state), out(result), 0);
                if(state.numFiles == 0) {
                    maxGap = EEPROM.length() - dataStart;
                    state.taskId = FreeSpaceState::End;
                }
                return Task::Pending();
            
            case FreeSpaceState::CollectAddresses:
                freeSpace_collectAddresses(out(state), out(result), 0);
                return Task::Pending();
        
            case FreeSpaceState::BubbleSortEntrys:
                freeSpace_bubbleSortEntrys(out(state), out(result), 0, false);
                if(state.taskId == FreeSpaceState::GetGap) {
                    maxGap = 0;
                    int firstGap =
                        (int)state.addressAt(0).start -
                        (int)dataStart;
                    if(firstGap > 0)
                        maxGap = firstGap;
                }
                return Task::Pending();
            
            case FreeSpaceState::GetGap:
                largestFreeSpace_getGap(out(state), out(maxGap), out(result));
                return Task::Pending();

            case FreeSpaceState::End:
                return Task::Done();

            default: DEBUG_UNREACHABLE;
        }

        return Task::Done();
    }

    Task findFreeSpace(MutRef<FreeSpaceState> stateRef, MutRef<int> addressRef, u16 size) {
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
                freeSpace_bubbleSortEntrys(out(state), out(address), size, true);
                return Task::Pending();
            
            case FreeSpaceState::GetGap:
                findFreeSpace_getGap(out(state), out(address), size);
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
            address.ref = enoughSpace() ? dataStart : FREESPACE_ERROR;
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
            address.ref = FREESPACE_ERROR;
            state.taskId = FreeSpaceState::End;
            return;
        }

        state.addressAt(state.i).start = entry.address;
        state.addressAt(state.i).end = entry.address + entry.size;

        state.i++;
        return;
    }

    inline void freeSpace_bubbleSortEntrys(MutRef<FreeSpaceState> stateRef, MutRef<int> address, u16 size, bool isFindFreeSpace) {
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
            int firstGap =
                (int)state.addressAt(0).start -
                (int)dataStart;

            if(firstGap >= (int)size) {
                address.ref = dataStart;
                state.taskId = isFindFreeSpace ? FreeSpaceState::End : FreeSpaceState::GetGap;
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

    inline void largestFreeSpace_getGap(MutRef<FreeSpaceState> stateRef, MutRef<u16> maxGapRef, MutRef<int> address) {
        auto& maxGap = maxGapRef.ref;
        auto& state = stateRef.ref;    
        auto& i = state.i;

        auto endLoop = [&]() {return i >= state.numFiles - 1;};

        if(endLoop()) {

            int tailGap =
                EEPROM.length() -
                state.addressAt(state.numFiles - 1).end;

            if(tailGap > (int)maxGap)
                maxGap = tailGap;

            address.ref = maxGap;
            state.taskId = FreeSpaceState::End;
            return;
        }

        int gap =
            (int)state.addressAt(i+1).start -
            (int)state.addressAt(i).end;

        if(gap > (int)maxGap)
            maxGap = gap;

        i++;
    }

    inline void findFreeSpace_getGap(MutRef<FreeSpaceState> stateRef, MutRef<int> address, u16 size) {
        auto& state = stateRef.ref;
        auto& i = state.i;
        auto endLoop = [&]() {return i >= state.numFiles-1;};

        if(endLoop()) {
            u16 tailGap = EEPROM.length() - state.addressAt(state.numFiles - 1).end;
            
            if(tailGap >= size)
                address.ref = state.addressAt(state.numFiles - 1).end;
            else 
                address.ref = FREESPACE_ERROR;
            
            state.taskId = FreeSpaceState::End;
            return;
        }

        int gap =
            (int)state.addressAt(i+1).start -
            (int)state.addressAt(i).end;
        
        if(gap >= (int)size) {
            address.ref = state.addressAt(i).end;
            state.taskId = FreeSpaceState::End;
            return; 
        }
        i++;
    }
}