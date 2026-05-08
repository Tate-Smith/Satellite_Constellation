/*
File: Terminal
Date Created: April 29th, 2026
Last Updated: May 7th, 2026
Author: Tate Smith
Purpose: This file is the display for the ground control, everytime a satellite sends a data dump it is updated so it
can see the most recent telemtery stats for the Satellite, it also allows for the user to send command via input
*/

#include "Terminal.h"

Terminal::Terminal(MessageQueue<SatOutput> *outputQueue, MessageQueue<CommandInput> *inputQueue, std::atomic<bool> *running) : 
outputQueue(outputQueue), inputQueue(inputQueue), running(running) {
    assert(running != nullptr);
    assert(outputQueue != nullptr);
    assert(inputQueue != nullptr);
}

void Terminal::addSat(std::unique_ptr<SatelliteData> sat) {
    /*
    This method takes in a unique pointer to a satellite data object and adds it to the satellite data map
    */
    assert(sat);
    sats.emplace(sat->getSatId(), std::move(sat));
}

void Terminal::runQueueThread(std::mutex& mtx) {
    /*
    This method takes in a mutex, and it runs a seprate queue thread that listens for messages on the output queue
    and when it recieves them it updates the correct data object with the new information
    */
    assert(outputQueue != nullptr);
    assert(!sats.empty());
    assert(running != nullptr);
    // create a thread to listen for new messages on the outputQueue
    output_queue_thread = std::thread([&] {
        SatOutput msg;
        while (running->load()) {
            msg = outputQueue->pop();
            if (!running->load()) break;

            {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = sats.find(msg.id);
                if (it == sats.end()) continue;
                SatelliteData *sat = it->second.get();
                if (!msg.chunks.empty()) sat->logData(msg.chunks); // log all the data thats in the struct
                if (msg.dump_msg == 1) {
                    sat->updateVals(msg.x, msg.y, msg.z, msg.vx, msg.vy, msg.vz);
                    sat->updatePackets(msg.packetsReceived, msg.packetsSent);
                }
                sat->markAlive(msg.alive);
            }
        }
    });
}

bool Terminal::checkRegex(const std::string &input, std::string *errorMsg) {
    /*
    This method takes in two strings being input and errorMsg, it then checks the input against a regex
    and determines if the format is correct, if not it fills out the error message
    */
    assert(errorMsg != nullptr);
    // make sure the command is valid via regex
    static const std::regex pattern(R"(\d+ -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)? -?\d+(\.\d+)?)");
    if (!std::regex_match(input, pattern)) {
        *errorMsg = "Invalid format; Format: id x y z vx vy vz; or quit";
        return false;
    }
    return true;
}

void Terminal::initInput(const std::string &input, std::string *errorMsg) {
    /*
    This method takes in two strings being input and errorMsg, and it prints out the error message and input
    at the bottom two lines of the screen
    */
    assert(errorMsg != nullptr);
    // prints status messages above the input line
    mvprintw(LINES-2, 0, "%s", errorMsg->c_str());
    clrtoeol(); // clears input line
    // input line at the bottom
    mvprintw(LINES-1, 0, "> %s", input.c_str());
    clrtoeol(); // clears input line
    move(LINES-1, 2 + (int)input.size());
    refresh();
}

void Terminal::run() {
    /*
    This method is ran by the main function and sets the screen for both the input and the output of the function,
    the output being the top of the screen and the input the very bottom, it also handles quitting with a graceful
    shutdown
    */
    assert(outputQueue != nullptr);
    assert(inputQueue != nullptr);
    assert(running != nullptr);
    initscr(); // intializes ncurses
    cbreak(); // charcaters are sent immedeately without enter
    noecho(); // doesn't automatically print characters to screen
    keypad(stdscr, TRUE); // enables spacial keys
    timeout(100); // this sets the timeout period for blocking wating for user input
    // in the main thread read user input into an input buffer and add those commands to the input_queue
    std::string input;
    std::string errorMsg;

    std::mutex mtx;
    runQueueThread(mtx);
    while (running->load()) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            printAllSats();
        }

        initInput(input, &errorMsg);

        int c = getch();
        if (c == ERR) continue; // timout occured
        if (c == '\n' || c == '\r') {
            // if the user hits enter, so the input is done
            if (input == "quit") {
                // exit the program
                *running = false;
                outputQueue->shutdown();
                inputQueue->shutdown();
                break;
            }
            if (!checkRegex(input, &errorMsg)) continue;
            errorMsg = "";
            CommandInput com = parser.commandDecoder(input);
            if (!sats.contains(com.id)) errorMsg = "Satellite with id: " + std::to_string(com.id) + " does not exist\n";
            else inputQueue->pushBack(com);
            input.clear();
        }
        // register deltion of characters too
        else if (c == KEY_BACKSPACE || c == 127 || c == KEY_DC) {
            if (!input.empty()) input.pop_back();
        }
        else if (c >= 32 && c < 127) input += (char) c;
    }
    endwin(); // restores terminal to normal
    // shut down queue thread
    output_queue_thread.join();
}

void Terminal::printAllSats() {
    /*
    This method prints out all the information from the satellite data objects to the top of the screen
    */
    assert(!sats.empty());
    int rows, cols;
    getmaxyx(stdscr, rows, cols); // get terminal dimension
    (void)rows;
    clear(); // clears the entire screen
    int y = 0;
    mvprintw(y++, 0, "<------------------------------------------------------------------------ Satellite Network ------------------------------------------------------------------------>");
    for (const auto &sat : sats) {
        SatelliteData &s = *sat.second;
        mvprintw(y++, 0, "%s", s.toString().c_str());
    }
    mvprintw(y++, 0, "<------------------------------------------------------------------------ Satellite Network ------------------------------------------------------------------------>");
}