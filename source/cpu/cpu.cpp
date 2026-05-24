#include "cpu.hpp"
#include <fstream> 
#include <cstdint>

chip_8::chip_8(){
  pc = chip_8::START_ADDRESS;
  for(unsigned int i = 0; i < FONTSET_SIZE; ++i){
    memory[chip_8::FONTSET_START_ADDRESS + i] = fontset[i];
  }
}

void chip_8::load_ROM(char const* filename){

  std::ifstream file(filename, std::ios_base::binary | std::ios::ate);

  if (file.is_open()){
    std::streampos size = file.tellg();
    char* buffer = new char[size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    for(long i = 0; i < size; ++i)
      chip_8::memory[chip_8::START_ADDRESS + i] = buffer[i];
    delete[] buffer;
  }
}
