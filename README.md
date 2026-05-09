# Satellite Constellation Simulator

A multi-process, real-time satellite network simulator built in C++20. A ground control station communicates with a configurable constellation of satellite nodes over live UDP sockets exchanging telemetry, heartbeats, course-correction commands, and acknowledgements across independently threaded processes.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      Ground Control                         │
│                                                             │
│  Listener Thread ──► Receiver ──► OutputQueue               │
│  Sender Thread   ──► GCConnectionHandler ──► UDP Sockets    │
│  Update Thread   ──► Reconnection / Timeout Logic           │
│  Logger Thread   ──► File Output                            │
│  Main Thread     ──► ncurses Terminal UI                    │
└────────────────────────┬────────────────────────────────────┘
                         │  UDP
         ┌───────────────┼───────────────┐
         │               │               │
┌────────▼──────┐ ┌──────▼──────┐ ┌─────▼───────┐
│  Satellite 1  │ │ Satellite 2 │ │ Satellite N │
│               │ │             │ │             │
│ Sim Thread    │ │  Sim Thread │ │  Sim Thread │
│ Listener      │ │  Listener   │ │  Listener   │
│ Sender        │ │  Sender     │ │  Sender     │
│ Logger        │ │  Logger     │ │  Logger     │
└───────┬───────┘ └──────┬──────┘ └──────┬──────┘
        │                │               │
        └────────────────┴───────────────┘
                  Peer-to-peer UDP mesh
```

Each satellite is a standalone process. The ground control is a separate process. All communication is over real UDP sockets on localhost or a network.

---

## Features

- **Live UDP networking** - real socket communication between independent processes, no simulation stubs
- **Binary message protocol** - custom serialization/deserialization for four message types: `HEARTBEAT`, `FILE_MSG`, `ACK`, `COMMAND`
- **Multi-threaded architecture** - listener, sender, simulation, update, and logger threads running concurrently per process
- **Heartbeat & reconnection logic** - satellites are monitored for timeouts; ground control attempts reconnection with exponential backoff up to a configurable number of retries before marking a satellite dead
- **Telemetry data dumps** - each satellite periodically dumps its full log file to ground control over chunked UDP messages, which are reassembled and parsed at the receiver
- **Course correction commands** - ground control operator can send position and velocity override commands to any satellite via the terminal
- **Live ncurses terminal UI** - real-time telemetry dashboard with bottom-line command input
- **Thread-safe message queues** - condition variable-backed generic queue used for all inter-thread communication
- **Per satellite logging** - every satellite and the ground control write timestamped logs to individual output files

---

## Message Protocol

| Type | Direction | Purpose |
|---|---|---|
| `HEARTBEAT` | Both | Connection keepalive, initial handshake |
| `FILE_MSG` | Satellite → GC | Chunked telemetry log dump |
| `ACK` | Both | Acknowledgement of received message |
| `COMMAND` | GC → Satellite | Position/velocity course correction |

All messages share a common `Header` containing type and size, followed by sender ID and port. Each type is serialized to a compact binary byte buffer for transmission.

---

## Project Structure

```
.
├── src/
│   ├── config/             # Compile-time constants (ports, timeouts, buffer sizes)
│   │   └── Config.h
│   ├── protocol/           # Message definitions, serializer, protocol utils
│   ├── logging/            # Thread-safe logger
│   ├── concurrency/        # Generic thread-safe MessageQueue
│   ├── Satellite/
│   │   ├── core/           # Satellite, Simulation
│   │   └── network/        # NetworkManager, ConnectionHandler, PeerConnection
│   └── GroundControl/
│       ├── networking/     # Receiver, GCConnectionHandler, Connection
│       ├── output/         # Terminal (ncurses UI), Parser
│       └── datastorage/    # SatelliteData
├── Makefile
├── run_simulation.sh
└── README.md
```

---

## Configuration

All tunable system constants are defined in `src/config/Config.h`. Since all values are `constexpr`,
they are resolved entirely at compile time with zero runtime overhead.

| Constant | Default | Description |
|---|---|---|
| `GC_PORT` | `8000` | Ground control listening port |
| `BUFFER_SIZE` | `2048` | UDP receive buffer size (bytes) |
| `TIMEOUT_MS` | `100` | Socket receive timeout (milliseconds) |
| `HEARTBEAT_INTERVAL` | `5` | Seconds between satellite heartbeats |
| `DUMP_INTERVAL` | `2` | Number of heartbeats per dump |
| `RECONNECT_INTERVAL` | `10` | Seconds between reconnection attempts |
| `TIMEOUT_THRESHOLD` | `20` | Seconds before a satellite is deemed timed out |
| `MAX_RECONNECT_ATTEMPTS` | `10` | Reconnect attempts before marking satellite dead |
| `TIME_STEP` | `1.0` | Simulation tick rate in seconds |
| `MAX_FILE_CHUNKS` | `100` | Max UDP chunks per telemetry dump |
| `MAX_CONFIG_LINES` | `100` | Max satellites readable from SatSetup file |
| `FILE_CHUNK_SIZE` | `1024` | Bytes per file message chunk |

To change any value, edit `Config.h` and rebuild with `make`.

---

## Build

**Dependencies:** `g++` with C++20 support, `libncurses`

```bash
# Install ncurses if needed (Debian/Ubuntu)
sudo apt install libncurses-dev

# Build both binaries
make

# Build individually
make ground_control
make satellite

# Clean
make clean
```

The Makefile compiles with `-Wall -Wextra -Wpedantic -Wshadow -Werror -pthread -MMD -MP`.

---

## Running the Simulation

The included shell script launches a full 5-satellite constellation and the ground control automatically:

```bash
chmod +x run_simulation.sh
./run_simulation.sh
```

This starts 5 satellite processes with preset parameters, waits for their servers to initialize, then launches ground control connected to all five. When you quit ground control (`quit`), all satellite processes are terminated cleanly.

**To launch manually:**
```bash
# Satellite: id x y z vx vy vz port ip peer:ip:port ...
./run_satellite 1 137932.0 0.1278 0 1000.90 2368.1 1 5001 127.0.0.1 2:127.0.0.1:5002

# Ground Control: path to satellite config file
./run_ground_control SatSetup.txt
```

**Satellite config file format (`SatSetup.txt`):**
```
1 127.0.0.1 5001
2 127.0.0.1 5002
3 127.0.0.1 5003
```

---

## Ground Control Terminal

The ncurses UI displays a live telemetry dashboard for all connected satellites, updated as data dumps arrive. At the bottom input line, the operator can send course corrections:

```
> id x y z vx vy vz        # send command to satellite with given id
> quit                      # graceful shutdown of all threads
```

Example:
```
> 3 -263810.0 123278.0 120983.23 -2379.0 -0.1 0.238
```

Output:
<img width="1190" height="282" alt="Screenshot 2026-05-08 at 8 21 05 AM" src="https://github.com/user-attachments/assets/2cb0f940-1a62-4246-ab7e-5ccfe03ac8ba" />

---

## NASA JPL Power of Ten — Coding Standards Compliance

The [NASA/JPL Power of Ten rules](https://spinroot.com/gerard/pdf/P10.pdf) are safety-critical coding
standards written for C. This project is C++, so some rules address concerns modern C++ handles
structurally, but the spirit of each rule was considered throughout development.

**Rules 1, 4, 5, 6, 7, 8, 9, and 10 are fully followed.**

The three intentional deviations are:

**Rule 2 — Fixed Loop Bounds:** Core thread loops (`while (running->load())`) are unbounded by design,
they run for the lifetime of the program and exit via an atomic shutdown flag. Bounding them with a fixed
iteration count would break the architecture of a long-running networked system.

**Rule 3 — No Dynamic Allocation After Initialization:** C++ standard library types (`std::vector`,
`std::string`, `std::unique_ptr`) allocate on the heap throughout runtime. This is standard practice for
a modern C++ networked application. A flight-software port would replace these with static buffers and
fixed-size memory pools.

---

## Author

**Tate Smith**
