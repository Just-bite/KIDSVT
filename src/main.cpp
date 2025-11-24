#include "vmach.hpp"
#include "vram.hpp"
#include <iostream>
#include <string>

int main() {
    Vram vram(16);

    // Пример программы с правильным синтаксисом
     std::string program = R"(
    
    16 loop
        1 readi lshift
        readi write
        asc
    endloop
    
   
    16 loop
        readi           
        1 lshift        
        readi read     
        equal? assert!  
        asc
    endloop
)";

    Vmach vm(program, vram);
    
    // Вариант 1: Пошаговое выполнение
    std::cout << "=== Step-by-step execution ===" << std::endl;
    for (size_t k = 0; k < 300; k++) {
        vm.step();
        if (vm.is_halted()) break;
    }

    // Или Вариант 2: Полное выполнение
    // std::cout << "=== Full execution ===" << std::endl;
    // vm.run();

    std::cout << "Final VRAM:" << std::endl;
    for (size_t j = 0; j < vram.len; j++) {
        std::cout << std::hex << vram.read(j) << " ";
    }
    std::cout << std::endl;

    return 0;
}