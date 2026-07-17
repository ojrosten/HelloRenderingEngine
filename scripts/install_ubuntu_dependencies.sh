#!/bin/bash

# Exit immediately if a command fails
set -e

dependencies=(
    build-essential
    g++-15
    libtbb-dev
    cmake
    ninja-build
    lcov
    libwayland-dev
    pkg-config
    libxkbcommon-dev
    libx11-dev
    libxrandr-dev
    libxinerama-dev
    libxcursor-dev
    libxi-dev
    libdecor-0-dev
    libgl1-mesa-dev
    vulkan-tools
    libvulkan-dev
    vulkan-validationlayers
    spirv-tools
    glslang-tools
    glslc
)

echo "--- Ensuring Graphics PPA is present ---"
# Only add the PPA if it's not already in your sources
if ! grep -q "kisak/kisak-mesa" /etc/apt/sources.list.d/* 2>/dev/null; then
    # add-apt-repository lives in software-properties-common, which minimal images may lack
    if ! command -v add-apt-repository >/dev/null; then
        sudo apt update
        sudo apt install -y software-properties-common
    fi
    sudo add-apt-repository -y ppa:kisak/kisak-mesa
fi

echo "--- Updating Package Lists ---"
sudo apt update

echo "--- Installing Dependencies ---"
# -y for non-interactive, build-essential covers gcc/g++
sudo apt install -y "${dependencies[@]}"

echo "--- Verification ---"
# If the script reached this point thanks to 'set -e', it's successful.
echo "All dependencies installed successfully!"
