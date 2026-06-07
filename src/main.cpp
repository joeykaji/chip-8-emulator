#include "cpu/cpu.hpp"
#include "platform/platform.hpp"
#include <iostream>

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: chip8 <rom> [cycleRate]\n";
        return 1;
    }

    float cycleRate = (argc >= 3) ? std::stof(argv[2]) : 700.0f;
    int scale = 10; // window = 640x320

    chip_8 cpu(cycleRate);
    cpu.load_ROM(argv[1]);

    Platform platform("CHIP-8", 64 * scale, 32 * scale, 64, 32);

    while(cpu.isRunning()){
        if(!platform.processInput(cpu.getKeys())) break;
        platform.tickTimers(cpu.getDelayTimer(), cpu.getSoundTimer());
        cpu.cycle();
        platform.update(cpu.getVideo());
    }
    return 0;
}
