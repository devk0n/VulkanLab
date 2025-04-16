#!/bin/bash

# Description:
# Gathers all *.cpp and *.h files from source and include folders
# and includes the top-level CMakeLists.txt.
# Outputs everything to Combined.txt.

# Find all .cpp and .h files in the specified directories
allFiles=$(find ./source ./include -type f \( -name "*.cpp" -o -name "*.h" \))

# Output file
outputFile="Combined.txt"

# Remove previous output file if it exists
[ -f "$outputFile" ] && rm "$outputFile"

# Append contents of each file to the output with a header
for file in $allFiles; do
    echo -e "\n// -------- $file --------\n" >> "$outputFile"
    cat "$file" >> "$outputFile"
done
