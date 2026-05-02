// the .h file for the parser

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include <vector>
#include "OutputMessages.h"

class Parser {
    public:
        SatOutput dataDecoder(int id, std::vector<char> chunk, bool alive);
        CommandInput commandDecoder(std::string command);
};

#endif