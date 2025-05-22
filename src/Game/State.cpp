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
    for (auto &player : players) {
        player.updatePosition();
    }
}

void State::updatePlayer(Player playerUpdate) {
    Player* playerToUpdate = getPlayerWithId(playerUpdate.getId());
    playerToUpdate->updateSpeed(playerUpdate);
    playerToUpdate->setXPos(playerUpdate.getXPos());
    playerToUpdate->setYPos(playerUpdate.getYPos());
}

Player *State::getPlayerWithId(int id) {
    cout << "GEttimg player with id "<<id << "\n";
    for (auto &player : players) {
        if (player.getId() == id) {
            cout << "Found the player\n";
            return &player;
        }
    }
    cout << "Didnt find the player\n";
    return nullptr;
}

void State::drawState() {
    const int width = 100;
    const int height = 50;

    //move cursor to top left
    cout << "\033[H";

    std::vector<std::vector<char>> buffer(height, std::vector<char>(width, ' '));

    // Place players in the buffer
    for (Player player : players) {
        int x = player.getXPos();
        int y = player.getYPos();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            buffer[y][x] = '@';
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
    for (int i = 0; i < width; ++i) std::cout << "_";
    std::cout << "\n";
    /**
    for (Player player : players) {
        cout << "Player " << player.getId() << " is at pos " << player.getXPos() << ", " << player.getYPos() <<
        " with x speed " << player.getXSpeed() << " and y speed "<<player.getYSpeed() <<"\n";
    }*/
}

