#!/bin/bash

# 1. Define your source and destination directories
# Using "." means it will search the current folder and all subfolders
SRC_DIR="."
DEST_DIR="./flattened_submission"

# 2. Create the destination directory
mkdir -p "$DEST_DIR"

echo "Flattening project into $DEST_DIR..."

# 3. Find and copy the files
# -type f : Only look for files (ignore directories)
# -not -path : Exclude the destination folder, .git, and object files so we don't copy junk
# -exec cp : Copy the found files to the destination
# --backup=numbered : If file.cpp already exists, save the next one as file.cpp.~1~
find "$SRC_DIR" -type f \
    -not -path "*/\.git/*" \
    -not -path "$DEST_DIR/*" \
    -not -name "*.o" \
    -not -name "main" \
    -exec cp --backup=numbered "{}" "$DEST_DIR/" \;

echo "Done! Your flattened files are in the '$DEST_DIR' folder."