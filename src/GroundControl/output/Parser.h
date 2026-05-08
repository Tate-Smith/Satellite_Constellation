// the .h file for the parser

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include "OutputMessages.h"

class Parser {
    public:
        SatOutput dataDecoder(int id, const std::vector<char> &chunk, bool alive); // convert the file msgs into a SatOutput struct
        CommandInput commandDecoder(const std::string &command); // converts a string command into a CommandInput struct
};

#endif