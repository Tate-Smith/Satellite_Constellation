/*
File: Terminal
Date Created: April 9th, 2026
Last Updated: April 29th, 2026
Author: Tate Smith
Purpose: This file is the display for the ground control, everytime a satellite sends a data dump it is updated so it
can see the most recent telemtery stats for the Satellite
*/


#include "Terminal.h"
#include <regex>

void Terminal::addSat(std::unique_ptr<SatelliteData> sat) {
    sats.emplace(sat->getSatId(), std::move(sat));
}

void Terminal::updateSat(int id, std::vector<char> file) {
    // this method decodes the log file for the data
    auto satInfo = sats.find(id);
    SatelliteData &sat = *satInfo->second;
    std::string line;
    std::string data; // this string stores the last line of data given from the satellite

    for (char c : file) {
        if (c == '\n') {
            size_t start = line.find("] : ");
            std::string substring = line.substr(start + 4);

            // check what type of message is it
            if (substring.starts_with("SAT ")) {
                data = substring;
            }
            else if (substring.starts_with("[NETWORK] Message received from ")) {
                sat.updateRecieved();
            }
            else if (substring.starts_with("[NETWORK] Sent message to ")) {
                sat.updateSent();
            }
            line.clear();
        }
        else line.push_back(c);
    }
    // once the iteration through the file is finished update the satellites info
    if (!data.empty()) {
        // split the line up into its parts and update the SatelliteData info
        int satId;
        double x, y, z, vx, vy, vz;

        int parsed = std::sscanf(data.c_str(), "SAT %d POS %lf %lf %lf VEL %lf %lf %lf", &satId, &x, &y, &z, &vx, &vy, &vz);
        if (parsed == 7) {
            sat.updatePos(x, y, z);
            sat.updateVel(vx, vy, vz);
        }
        else std::cerr << "Parsing failed" << std::endl;
    }
}

void Terminal::printAllSats() {
    std::cout << std::endl;
    std::cout << "<------------------------------------------------------------------------ Satellite Network " 
    << "------------------------------------------------------------------------>" << std::endl;
    for (const auto &sat : sats) std::cout << sat.second->toString();
    std::cout << "<------------------------------------------------------------------------ Satellite Network " 
    << "------------------------------------------------------------------------>" << std::endl;
    std::cout << std::endl;
}

void Terminal::markSatDead(int id, bool b) {
    auto sat = sats.find(id);
    sat->second->markAlive(!b);
}
