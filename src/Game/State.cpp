//
// Created by scott on 20.05.2025.
//
#include "../../include/Game/State.h"

void State::addPlayer(Player player) {
    players.push_back(player);
}

void State::updateState() {
    for (auto player : players) {
        player.updatePosition();
    }
}


