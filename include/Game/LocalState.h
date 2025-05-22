//
// Created by scott on 20.05.2025.
//

#include "State.h"

#ifndef EKSAMEN_LOCALSTATE_H
#define EKSAMEN_LOCALSTATE_H
class LocalState {
public:
    LocalState() = default;
    explicit LocalState(int playerId);
    void setGamerId(int id);
    Player* getMyPlayer();
    State* getState();
private:
    State currentState;
    int thisPlayerId;
};
#endif //EKSAMEN_LOCALSTATE_H
