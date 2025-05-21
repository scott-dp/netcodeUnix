//
// Created by scott on 20.05.2025.
//
#include <iostream>
#include "../../include/Game/State.h"

using namespace std;
void State::addPlayer(Player player) {
    players.push_back(player);
}

void State::updateState() {
    //TODO do a legal movement check on client side, not server side
    for (auto player : players) {
        player.updatePosition();
    }
}

void State::updatePlayer(Player playerUpdate) {
    Player* playerToUpdate = getPlayerWithId(playerUpdate.getId());
    playerToUpdate->updateXSpeed(playerUpdate.getXSpeed());
    playerToUpdate->updateYSpeed(playerUpdate.getYSpeed());
    playerToUpdate->setXPos(playerUpdate.getXPos());
    playerToUpdate->setYPos(playerUpdate.getYPos());

}

Player *State::getPlayerWithId(int id) {
    for (auto &player : players) {
        if (player.getId() == id) {
            return &player;
        }
    }
    return nullptr;
}

void State::drawState() {
    const int width = 100;
    const int height = 100;

    //move cursor to top left
    cout << "\033[H";

    std::vector<std::vector<char>> buffer(height, std::vector<char>(width, ' '));

    // Place players in the buffer
    for (Player player : players) {
        int x = player.getXPos();
        int y = player.getYPos();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            buffer[y][x] = '@'; // or 'P', etc.
        }
    }

    //top line
    std::cout << " ";
    for (int i = 0; i < width; ++i) std::cout << "_";
    std::cout << "\n";

    for (int i = 0; i < height; ++i) {
        std::cout << "|";
        for (int j = 0; j < width; ++j) {
            std::cout << buffer[i][j];
        }
        std::cout << "|\n";
    }

    //bottom line
    std::cout << " ";
    for (int i = 0; i < width; ++i) std::cout << "‾";
    std::cout << "\n";
}

