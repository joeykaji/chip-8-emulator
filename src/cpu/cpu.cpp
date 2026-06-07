#include "cpu.hpp"
#include <cstring>
#include <fstream> 
#include <cstdint>
#include <chrono>
#include <random>

chip_8::chip_8(float cycleRate)
 : randGen(std::chrono::system_clock::now().time_since_epoch().count()),
   randByte(0, 255U),
   cycleRate(cycleRate),
   lastTime(std::chrono::high_resolution_clock::now())
{
  pc = chip_8::START_ADDRESS;
  for(unsigned int i = 0; i < FONTSET_SIZE; ++i){
    memory[chip_8::FONTSET_START_ADDRESS + i] = fontset[i];
  }
  std::memset(video, 0, sizeof(video));
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

double chip_8::getDeltaTime(){
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = now - lastTime;
    lastTime = now;
    return dt.count(); // returns seconds as a double
}

void chip_8::run(){
    double instructionBudget = 0.0;
    while(running){
        double dt = getDeltaTime();
        instructionBudget += dt * cycleRate;
        while(instructionBudget >= 1.0){
            cycle();
            instructionBudget -= 1.0;
        }
    }
}

void chip_8::cycle(){
  uint16_t next_instruction = fetch();
  decode_and_execute(next_instruction);
}

uint16_t chip_8::fetch(){
  uint16_t instruction = (uint16_t)memory[pc]<<8 | memory[pc+1];
  pc = pc + 2;
  return instruction;
}

void chip_8::decode_and_execute(uint16_t code){
    switch(code & 0xF000){
        case 0x0000:
            switch(code & 0x00FF){
                case 0x00E0:
                    OP_00E0();
                    break;
                case 0x00EE:
                    OP_00EE();
                    break;
            }
            break;
        case 0x1000:
            OP_1NNN(code);
            break;
        case 0x2000:
            OP_2NNN(code);
            break;
        case 0x3000:
            OP_3XNN(code);
            break;
        case 0x4000:
            OP_4XNN(code);
            break;
        case 0x5000:
            OP_5XY0(code);
            break;
        case 0x6000:
            OP_6XNN(code);
            break;
        case 0x7000:
            OP_7XNN(code);
            break;
        case 0x8000:
            switch(code & 0x000F){
                case 0x0000: OP_8XY0(code); break;
                case 0x0001: OP_8XY1(code); break;
                case 0x0002: OP_8XY2(code); break;
                case 0x0003: OP_8XY3(code); break;
                case 0x0004: OP_8XY4(code); break;
                case 0x0005: OP_8XY5(code); break;
                case 0x0006: OP_8XY6(code); break;
                case 0x0007: OP_8XY7(code); break;
                case 0x000E: OP_8XYE(code); break;
            }
            break;
        case 0x9000:
            OP_9XY0(code);
            break;
        case 0xA000:
            OP_ANNN(code);
            break;
        case 0xB000:
            OP_BNNN(code);
            break;
        case 0xC000:
            OP_CXNN(code);
            break;
        case 0xD000:
            OP_DXYN(code);
            break;
        case 0xE000:
            switch(code & 0x00FF){
                case 0x009E: OP_EX9E(code); break;
                case 0x00A1: OP_EXA1(code); break;
            }
            break;
        case 0xF000:
            switch(code & 0x00FF){
                case 0x0007: OP_FX07(code); break;
                case 0x000A: OP_FX0A(code); break;
                case 0x0015: OP_FX15(code); break;
                case 0x0018: OP_FX18(code); break;
                case 0x001E: OP_FX1E(code); break;
                case 0x0029: OP_FX29(code); break;
                case 0x0033: OP_FX33(code); break;
                case 0x0055: OP_FX55(code); break;
                case 0x0065: OP_FX65(code); break;
            }
            break;
    }
}

void chip_8::OP_00E0(){
  std::fill(std::begin(video), std::end(video), 0);
}

void chip_8::OP_1NNN(uint16_t code){
  pc = code & 0xFFF;
}

void chip_8::OP_2NNN(uint16_t code){
  stack[sp] = pc;
  sp++;
  pc = code & 0xFFF;
}

void chip_8::OP_00EE(){
  sp--;
  pc = stack[sp];
}

void chip_8::OP_3XNN(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t NN = code & 0xFF;
  if(registers[x] == NN){
    pc += 2;
  }
}

void chip_8::OP_4XNN(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t NN = code & 0xFF;
  if(registers[x] != NN){
    pc += 2;
  }

}

void chip_8::OP_5XY0(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  if(registers[x] == registers[y]){
    pc += 2;
  }
}

void chip_8::OP_9XY0(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  if(registers[x] != registers[y]){
    pc += 2;
  }
}

void chip_8::OP_6XNN(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  registers[x] = code & 0xFF;
}

void chip_8::OP_7XNN(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  registers[x] +=  code&0xFF;
}

void chip_8::OP_8XY0(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[x] = registers[y];
}

void chip_8::OP_8XY1(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[x] = registers[x] | registers[y];
}

void chip_8::OP_8XY2(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[x] = registers[x] & registers[y];
}

void chip_8::OP_8XY3(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[x] = registers[x] ^ registers[y];
}

void chip_8::OP_8XY4(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  uint16_t res = registers[x] + registers[y];
  registers[0xF] = (res > 0xFF) ? 1 : 0;
  registers[x] = res & 0xFF;
}

void chip_8::OP_8XY5(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[0xF] = (registers[x] >= registers[y]) ? 1 : 0;
  registers[x] = registers[x] - registers[y];
}

void chip_8::OP_8XY7(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[0xF] = (registers[y] >= registers[x]) ? 1 : 0;
  registers[x] = registers[y] - registers[x];
}

void chip_8::OP_8XY6(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[0xF] = (registers[y] & 0x1);
  registers[x] = registers[y] >> 1;
}
void chip_8::OP_8XYE(uint16_t code){
  uint8_t x = (code >> 8) & 0xF;
  uint8_t y = (code >> 4) & 0xF;
  registers[0xF] = (registers[y] >> 7) & 0x1;
  registers[x] = registers[y] << 1;
}

void chip_8::OP_ANNN(uint16_t code){
  idx = code & 0xFFF;
}

void chip_8::OP_BNNN(uint16_t code){
  uint16_t target = code & 0xFFF;
  pc = target + registers[0];
}

void chip_8::OP_CXNN(uint16_t code){
  uint8_t x = code >> 8 & 0xF; 
  uint8_t NN = code & 0xFF;
  registers[x] = randByte(randGen) & NN;
}

// Draw sprite
void chip_8::OP_DXYN(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    uint8_t y = (code >> 4) & 0xF;
    uint8_t n = code & 0xF;

    uint8_t xPos = registers[x] % 64;
    uint8_t yPos = registers[y] % 32;
    registers[0xF] = 0;

    for(uint8_t row = 0; row < n; row++){
        uint8_t spriteByte = memory[idx + row];
        for(uint8_t col = 0; col < 8; col++){
            if(spriteByte & (0x80 >> col)){
                uint8_t sx = (xPos + col) % 64;
                uint8_t sy = (yPos + row) % 32;
                uint32_t& pixel = video[sy * 64 + sx];
                if(pixel){
                registers[0xF] = 1;
                  pixel = 0x00000000;  // off = black
                } else {
                  pixel = 0xFFFFFFFF;  // on = white
                }
            }
        }
    }
}

// Skip if key in VX is pressed
void chip_8::OP_EX9E(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    if(key[registers[x]]) pc += 2;
}

// Skip if key in VX is not pressed
void chip_8::OP_EXA1(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    if(!key[registers[x]]) pc += 2;
}

// VX = delay timer
void chip_8::OP_FX07(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    registers[x] = delay_timer;
}

// Set delay timer = VX
void chip_8::OP_FX15(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    delay_timer = registers[x];
}

// Set sound timer = VX
void chip_8::OP_FX18(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    sound_timer = registers[x];
}

// idx += VX
void chip_8::OP_FX1E(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    idx += registers[x];
}

// Wait for key press, store in VX (blocking)
void chip_8::OP_FX0A(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    for(uint8_t i = 0; i < 16; i++){
        if(key[i]){
            registers[x] = i;
            return;
        }
    }
    pc -= 2; // no key pressed, repeat this instruction
}

// Set idx to font sprite address for digit in VX
void chip_8::OP_FX29(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    idx = FONTSET_START_ADDRESS + (registers[x] * 5); // each font is 5 bytes
}

// Store BCD of VX in memory[idx], memory[idx+1], memory[idx+2]
void chip_8::OP_FX33(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    uint8_t val = registers[x];
    memory[idx + 2] = val % 10;       // ones
    val /= 10;
    memory[idx + 1] = val % 10;       // tens
    memory[idx]     = val / 10;       // hundreds
}

// Store registers V0-VX into memory starting at idx
void chip_8::OP_FX55(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    for(uint8_t i = 0; i <= x; i++){
        memory[idx + i] = registers[i];
    }
}

// Read memory starting at idx into registers V0-VX
void chip_8::OP_FX65(uint16_t code){
    uint8_t x = (code >> 8) & 0xF;
    for(uint8_t i = 0; i <= x; i++){
        registers[i] = memory[idx + i];
    }
}
