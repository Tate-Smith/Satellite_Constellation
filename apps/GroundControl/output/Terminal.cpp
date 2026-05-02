/*
File: Terminal
Date Created: April 29th, 2026
Last Updated: April 29th, 2026
Author: Tate Smith
Purpose: This file is the display for the ground control, everytime a satellite sends a data dump it is updated so it
can see the most recent telemtery stats for the Satellite
*/


#include "Terminal.h"
#include <regex>
#include <thread>
#include <ncurses.h>

Terminal::Terminal(MessageQueue<SatOutput> *output_queue, MessageQueue<CommandInput> *input_queue) : output_queue(output_queue), 
input_queue(input_queue) {}

void Terminal::addSat(std::unique_ptr<SatelliteData> sat) {
    sats.emplace(sat->getSatId(), std::move(sat));
}

void Terminal::run() {
    initscr(); // intializes ncurses
    cbreak(); // charcaters are sent immedeately without enter
    noecho(); // doesn't automatically print characters to screen
    keypad(stdscr, TRUE); // enables spacial keys
    
    std::atomic<bool> running = true;
    std::mutex mtx;
    // create a thread to listen for new messages on the output_queue
    std::thread output_queue_thread([&] {
        while (running) {
            SatOutput msg = this->output_queue->pop();
            if (!running) break;
            {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = sats.find(msg.id);
                if (it != sats.end()) {
                    if (msg.dump_msg == 1) {
                        it->second->updatePos(msg.x, msg.y, msg.z);
                        it->second->updateVel(msg.vx, msg.vy, msg.vz);
                        it->second->updateRecieved(msg.packetsRecieved);
                        it->second->updateSent(msg.packetsSent);
                    }
                    it->second->markAlive(msg.alive);
                }
            }
        }
    });

    // in the main thread read user input into an input buffer and add those commands to the input_queue
    std::string input;
    int c;
    timeout(100); // this sets the timeout period for blocking wating for user input

    while (running) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            printAllSats();
        }

        mvprintw(LINES-1, 0, "> %s", input.c_str());
        clrtoeol(); // clears input line
        refresh();

        c = getch();
        if (c == ERR) continue; // timout occured
        if (c == '\n') {
            // if the user hits enter, so the input is done
            if (input == "quit") {
                // exit the program
                break;
            }
            // make sure the command is valid via regex
            std::regex pattern(R"(Command: SAT \d+ POS -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? VEL -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)?)");
            if (!std::regex_match(input, pattern)) {
                mvprintw(LINES-2, 0,"Invalid format; Format: Command SAT id POS x y z VEL vx vy vz");
                clrtoeol(); // clears input line
                input.clear();
                continue;
            }

            CommandInput com = parser.commandDecoder(input);
            input_queue->pushBack(com);
            input.clear();
        }
        // register deltion of characters too
        else if (c == KEY_BACKSPACE) if (!input.empty()) input.pop_back();
        else input += (char) c;

    }
    // shut down queue thread
    running = false;
    output_queue->shutdown();
    output_queue_thread.join();
    endwin(); // restores terminal to normal
}

void Terminal::printAllSats() {
    // prints tthe output to the output window
    int rows, cols;
    getmaxyx(stdscr, rows, cols); // get terminal dimension
    clear(); // clears the entire screen
    int y = 0;
    mvprintw(y++, 0, "<------------------------------------------------------------------------ Satellite Network ------------------------------------------------------------------------>");
    for (const auto &sat : sats) mvprintw(y++, 0, "%s", sat.second->toString().c_str());
    mvprintw(y++, 0, "<------------------------------------------------------------------------ Satellite Network ------------------------------------------------------------------------>");
}