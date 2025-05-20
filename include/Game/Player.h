//
// Created by scott on 19.05.2025.
//

#ifndef EKSAMEN_PLAYER_H
#define EKSAMEN_PLAYER_H

class Player {
private:
    int id;
    int xPos;
    int yPos;
public:
    Player(int id, int xPos, int yPos);
    int getId();
};
#endif //EKSAMEN_PLAYER_H
