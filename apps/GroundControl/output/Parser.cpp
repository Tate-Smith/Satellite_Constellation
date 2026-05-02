/*
File: Parser
Date Created: May 1st, 2026
Last Updated: May 2nd, 2026
Author: Tate Smith
Purpose: This file parses the output information from the reciver, and the commands from the user, into
structs for the program to use
*/

#include "Parser.h"

SatOutput Parser::dataDecoder(int id, std::vector<char> chunk, bool alive) {
    // this method decodes the log file for the data
    SatOutput out;
    out.id = id;
    if (chunk.empty()) {
        out.alive = alive;
        return out;
    }
    std::string line;
    std::string data; // this string stores the last line of data given from the satellite

    for (char c : chunk) {
        if (c == '\n') {
            size_t start = line.find("] : ");
            if (start == std::string::npos) {
                line.clear();
                continue;
            }
            std::string substring = line.substr(start + 4);

            // check what type of message is it
            if (substring.starts_with("SAT ")) {
                data = substring;
            }
            else if (substring.starts_with("[NETWORK] Message received from ")) {
                out.packetsReceived++;
            }
            else if (substring.starts_with("[NETWORK] Sent message to ")) {
                out.packetsSent++;
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
            out.x = x;
            out.y = y;
            out.z = z;
            out.vx = vx;
            out.vy = vy;
            out.vz = vz;
            out.dump_msg = 1;
            out.alive = alive;
        }
        else std::cerr << "Parsing failed" << std::endl;
    }

    return out;
}

CommandInput Parser::commandDecoder(std::string command) {
    // this method takes a command from the user and parses it into a commandInput struct
    CommandInput com;
    int satId;
    double x, y, z, vx, vy, vz;

    // get the command values
    int parsed = std::sscanf(command.c_str(), "%d %lf %lf %lf %lf %lf %lf", &satId, &x, &y, &z, &vx, &vy, &vz);
    if (parsed == 7) {
        com.id = satId;
        com.new_x = x;
        com.new_y = y;
        com.new_z = z;
        com.new_vx = vx;
        com.new_vy = vy;
        com.new_vz = vz;
    }
    else std::cerr << "Parsing failed" << std::endl;
    return com;
}
