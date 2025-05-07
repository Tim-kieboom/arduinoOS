/*
Tim Kieboom 1025003
*/
#include "EEPROM_handler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"
#include "hasFileStore/hasFileStore.h"
#include <utils/utils.h>

inline uint8_t getFATLength();
inline int getFATEntry_index(int index);
inline bool writeFATEntry(const FAT& fat);
inline uint8_t getFATEntry_size(int index);
inline uint16_t getFATEntry_addres(int index);
inline bool readFATEntry(int index, FAT& fat);
Task doesNameExist(const char* name, /*out*/bool& exist);
inline void err_retrieve_file_not_found(InputBuffer &input);
inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success);
inline void getFATEntry_name(int index, /*out*/char* name, uint8_t len, bool printableName = false);

Task commandFunc_store(InputBuffer &input) {
  static int processId = 0;
  static FAT fat = FAT();

  DEBUG_PRINTM(F("(debug only) store() current processId: "), processId, '\n');

  if (processId == 0) {
    if (!checkArguments(input, 3))
      return Done;

    ConstSpan<char> nameSpan = getToken(input, 1);

    if (nameSpan.isEmpty()) {
      Serial.println(F("!!error!! name is empty"));
      processId = 0;
      fat = FAT();
      return Done;
    }
    else if (nameSpan.len() > 11) {
      Serial.println(F("!!error!! name of file is to big, file names can not be bigger then 11 symbools"));
      processId = 0;
      fat = FAT();
      return Done;
    }
    fat.fillName(nameSpan);

    bool parseSuccess = false;
    fat.size = charSpanTo_uint8(getToken(input, 2), /*out*/ parseSuccess);
    DEBUG_PRINTM(
      F("(debug only) file.name: "), fat.fileName, '\n',
      F("(debug only) file.size: "), fat.size, '\n',
      F("(debug only) file.address: "), fat.address, '\n',
      F("(debug only) firstEmpty index: "), FileStore::getFirstEmpty(), '\n'
    );

    if (!parseSuccess) {
      Serial.println(F("!!error!! while trying to parse number in store command: uint8 overflow caught (number has an invalid symbools or is to big)"));
      processId = 0;
      fat = FAT();
      return Done;
    }

    processId++;
    return NotDone;
  }

  bool exists = false;
  Task nameTask = doesNameExist(fat.fileName, /*out*/exists);

  if(nameTask.isDone) {

    if (exists) {
      Serial.println(F("!!error!! fileName already exists"));
      processId = 0;
      fat = FAT();
      return Done;
    }
    
    bool isSuccess = writeFATEntry(fat);
    if(isSuccess) 
      printM(F("successfully stored file: '"), fat.fileName, F("'\n"));
    
    processId = 0;
    fat = FAT();
    return Done;
  } 

  return NotDone;
}

Task commandFunc_files(InputBuffer &input) {
  char name[BUFFER_SIZE+2];

  SmartArray<uint8_t> indexes = FileStore::getAllIndex();  
  IF_DEBUG(
    for(int i = 0; i < indexes.len(); i++)
      printM(indexes[i], ", ");
    Serial.println();
  )

  for(int i = 0; i < indexes.len(); i++) {
    uint8_t index = indexes[i];
    getFATEntry_name(index, /*out*/name, BUFFER_SIZE+1, /*printableName:*/true);
    printM('\"', name, "size(bytes): ", getFATEntry_size(index), F(", FAT index: "), index, '\n');
  }

  indexes.free();
  return Done;
}

Task commandFunc_retrieve(InputBuffer &input) {
  // const uint8_t len = getFATLength();
  // if(len == 0) {
  //   err_retrieve_file_not_found(input);
  //   return Done;
  // }

  ASSERT_TODO;
}

Task commandFunc_clearall(InputBuffer& input) {

  if(!checkArguments(input, 0))
    return Done;

  Serial.flush();
  Serial.print(F("this will DELETE ALL YOUR FILES are you sure (y)/(n) >> "));

  while(Serial.available() <= 0) {}
  
  char firstChar = Serial.read();
  Serial.println();
  if(firstChar != 'y') {
    Serial.println("clear cancelled");
    return Done;
  }

  for(uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM.update(i, 0);

  Serial.println(F("disk cleared"));
  Serial.flush();
  return Done;
}

Task commandFunc_freespace(InputBuffer &input) {
  ASSERT_TODO;
}

Task doesNameExist(const char* name, /*out*/bool& exist) {
  static uint8_t i = 0;
  exist = false;
  
  uint8_t len = getFATLength();
  if(len == 0)
    return Done;
  

  if(i >= len) {
    i = 0;
    return Done;
  }

  FAT fat;
  EEPROM.get(getFATEntry_index(i), /*out*/fat);
  if(strEquals(name, fat.fileName)) {
    i = 0;
    exist = true;
    return Done;
  }

  i++;
  return NotDone;
}

inline void err_retrieve_file_not_found(InputBuffer &input) {
  const char* name = getToken(input, 1).copy_asCstr();
  printM(F("!!error!! file name: '"), name, F("' is not found in system"));
}

inline bool writeFATEntry(const FAT& fat) {
  uint8_t FATlen = getFATLength();
  int fileIndex = FileStore::getFirstEmpty();

  if(fileIndex == -1) {
    Serial.println(F("!!error!! can not add a new file to system because system is already full"));
    return false;
  }

  int memIndex = getFATEntry_index(fileIndex);
  for(int i = 0; i < BUFFER_SIZE; i++)
    EEPROM.put(memIndex++, fat.fileName[i]);
  
  EEPROM.put(memIndex, fat.address);
  memIndex += 2;
  EEPROM.put(memIndex, fat.size);

  EEPROM.update(EEPROM_Header::NUM_FILES_INDEX, FATlen+1);
  FileStore::setFile(fileIndex, FileStore::HAS_FILE);

  ASSERT_EQ(FATlen+1, getFATLength());
  DEBUG_PRINTM(F("(debug only) FAT size: "), getFATLength(), '\n');
  return true;
}

inline bool readFATEntry(int index, FAT& fat) {
  if(index >= getFATLength())
    return false;

  getFATEntry_name(index, fat.fileName, sizeof(fat.fileName));
  fat.size = getFATEntry_size(index);
  fat.address = getFATEntry_addres(index);
  return true;
}

inline int getFATEntry_index(int index) {
  constexpr int firstFATIndex = EEPROM_Header::SIZE;
  return firstFATIndex + (SIZEOF_FAT * index);
}

inline void getFATEntry_name(int index, /*out*/char* name, uint8_t len, bool printableName) {
  int memIndex = getFATEntry_index(index);
  bool isEndOfName = false;

  for(uint8_t i = 0; i < len; i++) {
    if(isEndOfName) {
      name[i] = ' ';
      continue;
    }
    
    name[i] = EEPROM.read(memIndex++);
    if(name[i] == '\0') {
      isEndOfName = true;
      
      if(printableName) { 
        name[i] = '\"';
      }
      else {
        name[i] = '\0';
        return;
      }
    }
  }  
    
  name[len] = '\0';
}

inline uint8_t getFATEntry_size(int index) {
  return EEPROM.read(getFATEntry_index(index) + BUFFER_SIZE + sizeof(uint16_t));
}

inline uint16_t getFATEntry_addres(int index) {
  uint16_t addr;
  EEPROM.get(getFATEntry_index(index) + BUFFER_SIZE, /*out*/addr);
  return addr;
}

inline uint8_t getFATLength() {
  return EEPROM.read(EEPROM_Header::NUM_FILES_INDEX);
}

inline bool isNumber(int8_t currentDecimal) {
  return currentDecimal > -1 && currentDecimal < 10;
}

inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success) {
  uint16_t number = 0;
  success = false;

  for(size_t i = 0; i < str.len(); i++) {
    number *= 10;
    int8_t currentDecimal = (int8_t)(str[i] - '0');
    ASSERT_PRINT(isNumber(currentDecimal), str.copy_asCstr());
    if(!isNumber(currentDecimal))
      return 0;

    number += currentDecimal;
  }

  if(number < UINT8_MAX+1)
    success = true;

  return (uint8_t)number;
}

void FAT::fillName(ConstSpan<char>& name) {
  uint8_t i;
  uint8_t len = min(name.len(), BUFFER_SIZE-1);
  for(i = 0; i < len; i++) {
    fileName[i] = name[i];
  }
  fileName[len] = '\0'; 
}









