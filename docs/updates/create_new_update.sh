#!/bin/bash

# This script renames all files in the current directory with the following format: {number}_{month}_{day}.md
# It also creates a new file with the format: 1_{month}_{day}.md
# This script has not been tested yet.

# Get current date
month=$(date +"%m")
day=$(date +"%d")

# Get the highest number prefix
highest=0
for file in *.md; do
    num=${file%%_*}
    if (( num > highest )); then
        highest=$num
    fi
done

# Rename existing files
for (( num=highest; num>0; num-- )); do
    file=$(ls ${num}_*.md 2> /dev/null)
    if [[ -n $file ]]; then
        newnum=$((num+1))
        mv "$file" "${file/$num/$newnum}"
    fi
done

# Create new file
touch "1_${month}_${day}.md"