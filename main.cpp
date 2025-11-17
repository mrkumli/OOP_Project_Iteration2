#include "Game.h"
#include <iostream>

int main() {
    try {
        std::cout << "==================================" << std::endl;
        std::cout << "  HOT AND COLD - PHASE 1" << std::endl;
        std::cout << "  Movement & Death Mechanics" << std::endl;
        std::cout << "==================================" << std::endl;

        Game game;
        game.run();

        std::cout << "\nGame ended successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
