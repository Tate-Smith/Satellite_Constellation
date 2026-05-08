// This file is used to define certain constants throughout the file, and provide an simple way to adjust them globally

#ifndef CONFIG_H
#define CONFIG_H

namespace Config {
    // Network
    constexpr int GC_PORT = 8000;
    constexpr int BUFFER_SIZE = 2048;
    constexpr int TIMEOUT_MS = 100; // socket timeout

    // Timing
    constexpr int HEARTBEAT_INTERVAL = 10; // seconds between heartbeats
    constexpr int DUMP_INTERVAL = 2; // number of heartbeats per dump
    constexpr int RECONNECT_INTERVAL = 10; // seconds between reconnect attempts
    constexpr int TIMEOUT_THRESHOLD = 20; // seconds before satellite deemed timed out

    // Reconnection
    constexpr int MAX_RECONNECT_ATTEMPTS = 10;

    // Simulation
    constexpr double TIME_STEP = 1.0; // simulation tick

    // Logging / IO
    constexpr int MAX_FILE_CHUNKS = 100;
    constexpr int MAX_CONFIG_LINES = 100;
    constexpr int FILE_CHUNK_SIZE = 1024;
}

#endif
