#!/bin/bash

GREEN="\033[0;32m"
RESET_COLOR="\033[0m"

# starting
echo -e "${GREEN}TEST 1${RESET_COLOR}"

# booting the server
echo -e "${GREEN}[FIFO] Booting the server${RESET_COLOR}"
valgrind --leak-check=full build/server

sleep 3s

echo -e "${GREEN}[FIFO] Running some clients${RESET_COLOR}"
gnome-terminal --build/client
