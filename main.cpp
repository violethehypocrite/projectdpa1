#include "game.cpp"

#include <iostream>
#include <string>


int main() {
    std::string answer;

    do {
        Game game;
        game.run();

        std::cout << "\nMain lagi? (y/n): ";
        if (!(std::cin >> answer)) break;
    } while (answer == "y" || answer == "Y");

    std::cout << "Terima kasih sudah bermain!\n";
    return 0;
}
