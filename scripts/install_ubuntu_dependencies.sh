#!/bin/bash

dependencies=(
	gcc
	g++
	g++-14
	libtbb-dev
	cmake
	lcov
	libwayland-dev
	pkg-config
	libxkbcommon-dev
	libx11-dev
	libxrandr-dev
	libxinerama-dev
	libxcursor-dev
	libxi-dev)

sudo apt install ppa-purge
sudo ppa-purge ppa:kisak/kisak-mesa
sudo add-apt-repository ppa:kisak/kisak-mesa

# Update and upgrade the package lists
sudo apt update
sudo apt upgrade

# Install the dependencies
sudo apt install "${dependencies[@]}"

# Check if all dependencies are installed
for dependency in "${dependencies[@]}"; do
	if ! dpkg-query -W -f='${Status}\n' "$dependency" | grep -q "ok installed"; then
       		echo "Error: $dependency is not installed."
       		exit 1
  	fi
        done

echo "All dependencies installed successfully!"

exit 0
