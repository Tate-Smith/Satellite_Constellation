# File: Makefile
# Date Created: May 3rd, 2026
# Last Updated: May 3rd, 2026
# Author: Tate Smith
# Purpose: This make file compiles all the files necessary to run the program

CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Werror -pthread -MMD -MP
LDFLAGS  := -pthread
NCURSES  := -lncurses

# ── Shared sources ────────────────────────────────────────────────────────────
SHARED_SRCS := \
	src/logging/Logger.cpp \
	src/concurrency/MessageQueue.cpp \
	src/protocol/Serializer.cpp

# ── Ground Control sources ────────────────────────────────────────────────────
GC_SRCS := \
	src/GroundControl/main.cpp \
	src/GroundControl/networking/Connection.cpp \
	src/GroundControl/networking/GCConnectionHandler.cpp \
	src/GroundControl/networking/Receiver.cpp \
	src/GroundControl/output/Parser.cpp \
	src/GroundControl/output/Terminal.cpp \
	src/GroundControl/datastorage/SatelliteData.cpp

# ── Satellite sources ─────────────────────────────────────────────────────────
SAT_SRCS := \
	src/Satellite/main.cpp \
	src/Satellite/core/Satellite.cpp \
	src/Satellite/core/Simulation.cpp \
	src/Satellite/network/ConnectionHandler.cpp \
	src/Satellite/network/NetworkManager.cpp \
	src/Satellite/network/PeerConnection.cpp

# ── Object files ──────────────────────────────────────────────────────────────
BUILD_DIR := build

GC_OBJS     := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(GC_SRCS))
SAT_OBJS    := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SAT_SRCS))
SHARED_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SHARED_SRCS))

# ── Targets ───────────────────────────────────────────────────────────────────
.PHONY: all satellite ground_control clean

all: output ground_control satellite

output:
	@mkdir -p output

ground_control: $(GC_OBJS) $(SHARED_OBJS)
	$(CXX) $(LDFLAGS) $^ $(NCURSES) -o run_ground_control

satellite: $(SAT_OBJS) $(SHARED_OBJS)
	$(CXX) $(LDFLAGS) $^ -o run_satellite

# ── Generic compile rule ──────────────────────────────────────────────────────
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

ALL_OBJS := $(GC_OBJS) $(SAT_OBJS) $(SHARED_OBJS)
-include $(ALL_OBJS:.o=.d)

# ── Clean ─────────────────────────────────────────────────────────────────────
clean:
	rm -rf $(BUILD_DIR) run_ground_control run_satellite