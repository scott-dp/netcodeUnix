//
// Created by scott on 19.05.2025.
//

#ifndef EKSAMEN_PLAYER_H
#define EKSAMEN_PLAYER_H

#include <string>

using namespace std;

class Player {
private:
    int id;
    int xPos;
    int yPos;
    int xSpeed;
    int ySpeed;
public:
    void setId(int id);
    int getXSpeed();
    int getYSpeed();
    int getXPos();
    int getYPos();
    void setXPos(int newXPos);
    void setYPos(int newYPos);
    Player() = default;
    Player(int id, int xPos, int yPos);
    int getId();
    void updateXSpeed(int xSpeed);
    void updateYSpeed(int ySpeed);
    void updatePosition();
    string serialize();
    static Player deserialize(const string& serializedPlayer);
};
#endif //EKSAMEN_PLAYER_H
