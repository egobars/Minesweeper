#include "game/minesweeper.h"
#include <iostream>

int main() {
    Minesweeper minesweeper(0, 0, 0);
    std::cout << "Starting new game..." << std::endl;
    std::cout << "Enter difficulty: Easy/Medium/Hard" << std::endl;
    std::string command;
    std::cin >> command;
    if (command == "Easy") {
        minesweeper.NewGame(5, 5, 5);
    } else if (command == "Medium") {
        minesweeper.NewGame(10, 10, 30);
    } else if (command == "Hard") {
        minesweeper.NewGame(20, 30, 50);
    } else {
        std::cout << "Incorrect command." << std::endl;
        return 1;
    }
    while (minesweeper.GetGameStatus() != Minesweeper::GameStatus::VICTORY &&
           minesweeper.GetGameStatus() != Minesweeper::GameStatus::DEFEAT) {
        std::cout << "Type 'open [x] [y]' to open (x, y) cell, type 'mark [x] [y]' to mark (x, y) cell:" << std::endl;
        std::cin >> command;
        size_t x, y;
        std::cin >> x >> y;
        if (command == "open") {
            minesweeper.OpenCell({x, y});
        } else {
            minesweeper.MarkCell({x, y});
        }
        Minesweeper::RenderedField field = minesweeper.RenderField();
        for (const std::string& line : field) {
            std::cout << line << std::endl;
        }
    }
    if (minesweeper.GetGameStatus() == Minesweeper::GameStatus::VICTORY) {
        std::cout << "Congratulations, you are win!" << std::endl;
    } else {
        std::cout << "BOOM! You are lose." << std::endl;
    }
    return 0;
}