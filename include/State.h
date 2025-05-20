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
    void addPlayer(Player player);
    State parseState(string state);
private:
};


#endif //EKSAMEN_STATE_H
