#!/bin/bash
# File: sun_simulation.sh
# Date Created: May 3rd, 2026
# Last Updated: May 3rd, 2026
# Author: Tate Smith
# Purpose: This script file is meant to setup the whole simulation, with 5 satellites, and one ground control. 
# it sets up all the peers correctly for the satellites and creates the correct setup file for the ground control

GC_IP="127.0.0.1"
GC_PORT=8000

# Satellite ports
P1=5001; P2=5002; P3=5003; P4=5004; P5=5005

# Launch each satellite:
# ./run_satellite id x y z vx vy vz port gcip peer:ip:port ...
echo "Starting satellites..."

./run_satellite 1  137932.0 0.1278 0 1000.90 2368.1 1 $P1 $GC_IP  2:$GC_IP:$P2 3:$GC_IP:$P3 4:$GC_IP:$P4 5:$GC_IP:$P5 &
./run_satellite 2  1237234.00228 1000 0.2389 23672.1 236 26 $P2 $GC_IP  1:$GC_IP:$P1 3:$GC_IP:$P3 4:$GC_IP:$P4 5:$GC_IP:$P5 &
./run_satellite 3  -263810.0 123278 120983.23 -2379 -0.1 0.238 $P3 $GC_IP  1:$GC_IP:$P1 2:$GC_IP:$P2 4:$GC_IP:$P4 5:$GC_IP:$P5 &
./run_satellite 4  12 -1000 0 0.0 -0.1  0.1326 $P4 $GC_IP 1:$GC_IP:$P1 2:$GC_IP:$P2 3:$GC_IP:$P3 5:$GC_IP:$P5 &
./run_satellite 5  0 236823 1000 121.0 2329.89 0.1 $P5 $GC_IP  1:$GC_IP:$P1 2:$GC_IP:$P2 3:$GC_IP:$P3 4:$GC_IP:$P4 &

# Give satellites a moment to start their servers before GC connects
sleep 1

# Write the ground control config file
cat > SatSetup.txt <<EOF
1 $GC_IP $P1
2 $GC_IP $P2
3 $GC_IP $P3
4 $GC_IP $P4
5 $GC_IP $P5
EOF

echo "Starting ground control..."
./run_ground_control SatSetup.txt

# When ground control exits (user types quit), kill all satellites
echo "Shutting down satellites..."
kill $(jobs -p) 2>/dev/null
wait
echo "Simulation stopped."