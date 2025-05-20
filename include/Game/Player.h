//
// Created by scott on 19.05.2025.
//

#ifndef EKSAMEN_PLAYER_H
#define EKSAMEN_PLAYER_H

using namespace std;

class Player {
private:
    int id;
    int xPos;
    int yPos;
    int xSpeed;
    int ySpeed;
public:
    Player(int id, int xPos, int yPos);
    int getId();
    void updateXSpeed(int xSpeed);
    void updateYSpeed(int ySpeed);
    void updatePosition();
    string serialize();
};
#endif //EKSAMEN_PLAYER_H
