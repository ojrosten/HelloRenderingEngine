#!/bin/bash

# Path to your suppression file
SUPPRESS_FILE="../../sanitizers/ubuntu/lsan_suppressions.txt"

# Your executable
EXECUTABLE="./TestAll"

# Set LSAN_OPTIONS
export LSAN_OPTIONS="suppressions=$SUPPRESS_FILE"

# Run your program
$EXECUTABLE

# Unset LSAN_OPTIONS after running
unset LSAN_OPTIONS

