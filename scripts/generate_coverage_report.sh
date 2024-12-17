#!/bin/bash

# Check if a test directory was provided as an argument
if [[ -z "$1" ]]; then
  echo "Usage: $0 <test_directory>"
  exit 1
fi

# Get the directory of the script
script_dir=$(dirname "$0")

# Get the test directory from the first argument
test_dir="$1"

# Relative location of the html output directory
output_dir="${script_dir}/../coverage_reports"

# Create output directory if it doesn't exist
mkdir -p "$output_dir"

# Run ctest in the specified directory
cd "$test_dir"
ctest -T Test -T Coverage

# Generate lcov coverage report
lcov --directory .  --capture --output-file coverage.info

# Generate HTML report
genhtml --demangle-cpp -o "${output_dir}" coverage.info
