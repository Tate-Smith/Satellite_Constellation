/*
File: Terminal
Date Created: April 29th, 2026
Last Updated: April 29th, 2026
Author: Tate Smith
Purpose: This file is the display for the ground control, everytime a satellite sends a data dump it is updated so it
can see the most recent telemtery stats for the Satellite
*/


#include "Terminal.h"

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
    timeout(100); // this sets the timeout period for blocking wating for user input
    
    std::atomic<bool> terminalRun = true;
    std::mutex mtx;
    // create a thread to listen for new messages on the output_queue
    std::thread output_queue_thread([&] {
        while (terminalRun) {
            SatOutput msg = this->output_queue->pop();
            if (!terminalRun) break;

            {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = sats.find(msg.id);
                if (it != sats.end()) {
                    if (msg.dump_msg == 1) {
                        it->second->updateVals(msg.x, msg.y, msg.z, msg.vx, msg.vy, msg.vz);
                        it->second->updatePackets(msg.packetsReceived, msg.packetsSent);
                    }
                    it->second->markAlive(msg.alive);
                }
            }
        }
    });

    // in the main thread read user input into an input buffer and add those commands to the input_queue
    std::string input;
    std::string errorMsg;
    int c;

    while (terminalRun) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            printAllSats();
        }

        // prints status messages above the input line
        mvprintw(LINES-2, 0, "%s", errorMsg.c_str());
        clrtoeol(); // clears input line

        // input line at the bottom
        mvprintw(LINES-1, 0, "> %s", input.c_str());
        clrtoeol(); // clears input line

        move(LINES-1, 2 + (int)input.size());

        refresh();

        c = getch();
        if (c == ERR) continue; // timout occured
        if (c == '\n' || c == '\r') {
            // if the user hits enter, so the input is done
            if (input == "quit") {
                // exit the program
                break;
            }
            // make sure the command is valid via regex
            std::regex pattern(R"(\d+ -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)?)");
            if (!std::regex_match(input, pattern)) {
                errorMsg = "Invalid format; Format: id x y z vx vy vz; or quit";
                input.clear();
                continue;
            }
            errorMsg = "";
            CommandInput com = parser.commandDecoder(input);
            input_queue->pushBack(com);
            input.clear();
        }
        // register deltion of characters too
        else if (c == KEY_BACKSPACE || c == 127 || c == KEY_DC) {
            if (!input.empty()) input.pop_back();
        }
        else if (c >= 32 && c < 127) input += (char) c;

    }
    // shut down queue thread
    terminalRun = false;
    output_queue->shutdown();
    output_queue_thread.join();

    endwin(); // restores terminal to normal
}

void Terminal::printAllSats() {
    // prints tthe output to the output window
    int rows, cols;
    getmaxyx(stdscr, rows, cols); // get terminal dimension
    (void)rows;
    clear(); // clears the entire screen
    int y = 0;
    mvprintw(y++, 0, "<------------------------------------------------------------------------ Satellite Network ------------------------------------------------------------------------>");
    for (const auto &sat : sats) mvprintw(y++, 0, "%s", sat.second->toString().c_str());
    mvprintw(y++, 0, "<------------------------------------------------------------------------ Satellite Network ------------------------------------------------------------------------>");
}