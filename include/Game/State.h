//
// Created by scott on 20.05.2025.
//

#ifndef EKSAMEN_STATE_H
#define EKSAMEN_STATE_H

#include <vector>
#include <string>
#include "Player.h"

using namespace std;

class State {
public:
    vector<Player> players;
    Player* getPlayerWithId(int id);
    void addPlayer(Player player);
    void updatePlayer(Player playerUpdate);
    void updateState();
    void drawState();
private:
    vector<int> getPlayers();
};


#endif //EKSAMEN_STATE_H
