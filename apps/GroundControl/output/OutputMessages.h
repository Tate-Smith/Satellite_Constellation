// structs for the parser to create based on the info provided

#include <cstdint>

struct SatOutput {
    // information for the terminal to display
    uint32_t id = 0;
    double x = 0, y = 0, z = 0, vx = 0, vy = 0, vz = 0;
    int packetsSent = 0, packetsRecieved = 0;
    int dump_msg = -1; // to let the terminal know if it is a file dump, or simply updating the alive status of the satellite
    bool alive = true;

};

struct CommandInput {
    // the command from the user
    uint32_t id;
    double new_x, new_y, new_z, new_vx, new_vy, new_vz;
};