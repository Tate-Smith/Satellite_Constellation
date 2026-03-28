/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 25th, 2026
Author: Tate Smith
Purpose: This file represents a Satellite node in the constellation, it can send and receive 
information from other satellites and ground control
*/

#include "Satellite.h"
#include <cmath>
using namespace std;

Satellite::Satellite(int id, double x, double y, double z, double vx, double vy, double vz) {
    this->id = id;
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
}

int Satellite::getId() {
    return id;
}

int Satellite::getX() {
    return x;
}

int Satellite::getY() {
    return y;
}

int Satellite::getZ() {
    return z;
}

void Satellite::update(double dt) {
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
}

double Satellite::distance(Satellite other) {
    return sqrt(pow(other.getX() - this->x, 2) + pow(other.getY() - this->y, 2) + pow(other.getZ() - this->z, 2));
}

void Satellite::print() {
    cout << "Satellite " << id << ": Position (" << x << ", " << y << ", " << z << ") Velocity (" << vx << ", " << vy << ", " << vz << ")" << endl;
}