//
// Created by scott on 20.05.2025.
//
#include <iostream>
#include "../../include/Game/LocalState.h"

Player *LocalState::getMyPlayer() {
    cout << "Getting player with id " << thisPlayerId << endl;
    for (auto &player : currentState.players) {
        cout << player.getId()<<endl;
        if (player.getId() == thisPlayerId) {
            cout << "Found the player\n";
            return &player;
        }
    }
    cout << "Didint find the player locally with id " << thisPlayerId;
    return nullptr;
}

LocalState::LocalState(int playerId) {
    auto *myPlayer = new Player(playerId, 10, 10);
    this->currentState.addPlayer(*myPlayer);
    thisPlayerId = playerId;
}

void LocalState::setGamerId(int id) {
    Player *player = getMyPlayer();
    cout<<"found player with the id " << player->getId() <<endl;
    player->setId(id);
    this->thisPlayerId = id;
}

State *LocalState::getState() {
    return &currentState;
}
