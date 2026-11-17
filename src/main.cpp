#include <iostream>
#include "game.h"

using namespace sf;

int main(int argc, char** argv) {
    std::srand(std::time(nullptr));

    Game game;
    game.run();

    return 0;
}
