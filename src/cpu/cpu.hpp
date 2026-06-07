#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <random>
#include <chrono>

class chip_8{
  private:
    const unsigned int START_ADDRESS = 0x200;
    static const unsigned int FONTSET_SIZE = 80;
    const unsigned int FONTSET_START_ADDRESS = 0x50;
    bool running = true;
    float cycleRate;

    std::uint8_t registers[16]; //Each is a byte i make a list of 16 bytes as my registers
    std::uint8_t memory[4096]; //4096 bytes of RAM
    std::uint16_t idx; //One address for the memory address
    std::uint16_t pc; // Keeps track of the next program call
    std::uint16_t stack[16]; //16 program counters for functions
    std::uint8_t sp; // Where on the stack use a singular byte to represnt
    std::uint8_t delay_timer; // Regulates timing of game events
    std::uint8_t sound_timer; // Regulates timing of sound outpus
    std::uint8_t key[16]; // key on keyboard
    std::uint32_t video[64 * 32]; //1D array of 2048 pixels
    std::uint16_t opcode; // A specific instruction 4 hexs long
    
    uint8_t fontset[FONTSET_SIZE]=
    {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    std::chrono::high_resolution_clock::time_point lastTime;
    
    uint16_t fetch();
    void decode_and_execute(uint16_t code);
    double getDeltaTime();
    
    //Opcodes:
    void OP_00E0(); 
    void OP_1NNN(uint16_t code);
    void OP_2NNN(uint16_t code);
    void OP_00EE();
    void OP_3XNN(uint16_t code);
    void OP_4XNN(uint16_t code);
    void OP_5XY0(uint16_t code);
    void OP_9XY0(uint16_t code);
    void OP_6XNN(uint16_t code);
    void OP_7XNN(uint16_t code);
    void OP_8XY0(uint16_t code);
    void OP_8XY1(uint16_t code);
    void OP_8XY2(uint16_t code);
    void OP_8XY3(uint16_t code);
    void OP_8XY4(uint16_t code);
    void OP_8XY5(uint16_t code);
    void OP_8XY7(uint16_t code);
    void OP_8XY6(uint16_t code);
    void OP_8XYE(uint16_t code);
    void OP_ANNN(uint16_t code);
    void OP_BNNN(uint16_t code);
    void OP_CXNN(uint16_t code);
    void OP_DXYN(uint16_t code);
    void OP_EX9E(uint16_t code);
    void OP_EXA1(uint16_t code);
    void OP_FX07(uint16_t code);
    void OP_FX15(uint16_t code);
    void OP_FX18(uint16_t code);
    void OP_FX1E(uint16_t code);
    void OP_FX0A(uint16_t code);
    void OP_FX29(uint16_t code);
    void OP_FX33(uint16_t code);
    void OP_FX55(uint16_t code);
    void OP_FX65(uint16_t code);
          
  public:
        
    chip_8(float cycleRate);
    void run();
    void load_ROM(char const* filename); 
    bool        isRunning()     { return running; }
    uint8_t*    getKeys()       { return key; }
    uint32_t*   getVideo()      { return video; }
    uint8_t&    getDelayTimer() { return delay_timer; }
    uint8_t&    getSoundTimer() { return sound_timer; }
    void        cycle();
};
#endif
