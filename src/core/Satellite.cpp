/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 25th, 2026
Author: Tate Smith
Purpose: This file represents a Satellite node in the constellation, it can send and receive 
information from other satellites and ground control
*/

#include "Satellite.h"
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

void Satellite::update(double dt) {
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
}

void Satellite::print() {
    cout << "Satellite " << id << ": Position (" << x << ", " << y << ", " << z << ") Velocity (" << vx << ", " << vy << ", " << vz << ")" << endl;
}