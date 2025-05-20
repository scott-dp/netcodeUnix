//
// Created by scott on 19.05.2025.
//

#include "../../include/Game/Player.h"

#include <stdexcept>
#include <sstream>

using namespace std;

const int maxMapWidth = 100;
const int maxMapHeight = 100;

Player::Player(int id, int xPos, int yPos) : id(id) {
    if (xPos < 0 || xPos > maxMapWidth) {
        std::ostringstream oss;
        oss << "x position cannot be less than 0 or greater than " << maxMapWidth << " and is " << xPos;
        throw runtime_error(oss.str());
    }
    if (yPos < 0 || yPos > maxMapHeight) {
        std::ostringstream oss;
        oss << "y position cannot be less than 0 or greater than " << maxMapHeight << " and is " << yPos;
        throw runtime_error(oss.str());
    }
    this->xPos = xPos;
    this->yPos = yPos;
};