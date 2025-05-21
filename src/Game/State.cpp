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

}


