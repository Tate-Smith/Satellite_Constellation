## Satellite Constellation ##

satellite-constellation/
│
├── CMakeLists.txt
├── README.md
│
├── src/
│   ├── main.cpp
│   │
│   ├── core/
│   │   ├── Satellite.h / .cpp
│   │   ├── Simulation.h / .cpp
│   │
│   ├── network/
│   │   ├── NetworkManager.h / .cpp
│   │   ├── Connection.h / .cpp
│   │   ├── SocketUtils.h / .cpp
│   │
│   ├── protocol/
│   │   ├── Message.h
│   │   ├── Serializer.h / .cpp
│   │
│   ├── system/
│   │   ├── ThreadPool.h / .cpp
│   │   ├── Logger.h / .cpp
│   │
│   └── utils/
│       ├── Config.h / .cpp
│
├── apps/
│   ├── satellite/
│   │   └── main.cpp
│   └── ground_control/
│       └── main.cpp
│
└── scripts/
    ├── run_cluster.sh
