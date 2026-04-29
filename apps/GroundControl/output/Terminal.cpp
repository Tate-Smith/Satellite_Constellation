


#include "Terminal.h"

void Terminal::addSat(std::unique_ptr<SatelliteData> sat) {
    sats.emplace(sat->getSatId(), std::move(sat));
}

void Terminal::updateSat(int id, std::vector<char> file) {
    // this method decodes the log file for the data
    auto sat = sats.find(id);
    std::string line;
    std::string data; // this string stores the last line of data given from the satellite

    for (char c : file) {
        if (c == '\n') {
            size_t start = line.find("] : ");
            std::string substring = line.substr(start);

            // check what type of message is it
            if (substring.starts_with("Satellite ")) {
                data = substring;
            }
            else if (substring.starts_with("Message received from ")) {
                sat->second->updateRecieved();
            }
            else if (substring.starts_with("Sent message to ")) {
                sat->second->updateSent();
            }

        }
        else line.push_back(c);
    }
    // once the iteration through the file is finished update the satellites info
    // TODO
}

void Terminal::printAllSats() {
    std::cout << std::endl;
    std::cout << " <--------------------------------- Satellite Network --------------------------------->" << std::endl;
    for (const auto &sat : sats) std::cout << sat->toString();
    std::cout << " <--------------------------------- Satellite Network --------------------------------->" << std::endl;
    std::cout << std::endl;
}
