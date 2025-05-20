//
// Created by scott on 20.05.2025.
//
#include "../../include/Game/LocalState.h"

Player *LocalState::getMyPlayer() {
    //TODO implement
}

LocalState::LocalState(int playerId) {
    Player myPlayer(playerId, 10, 10);
    this->currentState.addPlayer(myPlayer);
}

void LocalState::setGamerId(int id) {
    this->thisPlayerId = id;
}
