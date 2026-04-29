// The .h file for the terminal screen

#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include "../datastorage/SatelliteData.h"

class Terminal {
    private:
        std::unordered_map<int, std::unique_ptr<SatelliteData>> sats;

    public:
        void addSat(std::unique_ptr<SatelliteData> sat); // adds a satellite to the vector
        void updateSat(int id, std::vector<char> file); // this method updates the satellites current info
        void printAllSats();
        void markSatDead(int id, bool b); // if a satllite dies it should say dead on the screen
};

#endif