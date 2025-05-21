//
// Created by scott on 19.05.2025.
//

#include "../../include/Game/Player.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

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
    xSpeed = 0;
    ySpeed = 0;
}

int Player::getId() {
    return id;
}

void Player::updateXSpeed(int newXSpeed) {
    //D keydown should call this with arg 1, A key should call this with arg -1
    ySpeed = 0;
    xSpeed = newXSpeed;
}

void Player::updateYSpeed(int newYSpeed) {
    //S keydown should call this with arg 1, W key should call this with arg -1
    xSpeed = 0;
    ySpeed = newYSpeed;
}

string Player::serialize() {
    std::ostringstream oss;
    oss << id << ' ' << xPos << ' ' << yPos << ' ' << xSpeed << ' ' << ySpeed;
    return oss.str();
}

void Player::updatePosition() {
    xPos += xSpeed;
    yPos += ySpeed;
}

Player Player::deserialize(const string& serializedPlayer) {
    cout << "Deserializing player: " << serializedPlayer << "\n";
    Player p;
    std::istringstream iss(serializedPlayer);
    iss >> p.id >> p.xPos >> p.yPos >> p.xSpeed >> p.ySpeed;
    cout << "Deserialize success\n";
    return p;
}

void Player::setId(int id) {
    this->id = id;
}

int Player::getXSpeed() {
    return xSpeed;
}

int Player::getYSpeed() {
    return ySpeed;
}

int Player::getXPos() {
    return xPos;
}

int Player::getYPos() {
    return yPos;
}

void Player::setXPos(int newXPos) {
    this->xPos = newXPos;
}

void Player::setYPos(int newYPos) {
    this->yPos = newYPos;
}

void Player::updateSpeed(Player updatedPlayer) {
    xSpeed = updatedPlayer.getXSpeed();
    ySpeed = updatedPlayer.getYSpeed();
}
