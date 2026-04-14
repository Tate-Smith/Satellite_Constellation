/*
File: SatelliteData
Date Created: April 9th, 2026
Last Updated: April 9th, 2026
Author: Tate Smith
Purpose: This file rholds the information for each satellite object the ground control is connected to
*/

#include "SatelliteData.h"

SatelliteData::SatelliteData(int id, const std::string &ip, int port) : id(id), ip(ip), port(port) {}

int SatelliteData::getId() { return this->id; }