#!/bin/bash

# Check if a test directory was provided as an argument
if [[ -z "$1" ]]; then
  echo "Usage: $0 <test_directory>"
  exit 1
fi

# Get the test directory from the first argument
test_dir="$1"

# Run ctest in the specified directory
cd "$test_dir"
ctest -T Test -T Coverage

# Generate lcov coverage report
lcov --directpry .  --capture --outputfile coverage.info

# Generate HTML report
genhtml --demangle-cpp -o coverage coverage.info
