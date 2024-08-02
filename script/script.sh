#!/bin/bash

CXX=g++

SRC_DIR="C:\Users\yugpa\OneDrive\Desktop\Projects\Programming\SFortinterp\SFort95-Interpreter\src"
BUILD_DIR= "C:\Users\yugpa\OneDrive\Desktop\Projects\Programming\SFortinterp\SFort95-Interpreter\build"
EXECUTABLE="test"

SRC_FILES=($(find "$SRC_DIR" -name "*.cpp"))

for SRC_FILE in "$SRC_DIR"; do
    OUTPUT_FILE = "${OUTPUT_DIR}/$(basename "${SRC_FILE}" .cpp).o"
    g++ -c "${SRC_FILE}" -o "${OUTPUT_FILE}"
    echo "Compiled ${SRC_FILE} to ${OUTPUT_FIL}"
done 

OBJ_FILES=($(find "$OUTPUT_DIR" -name "*.o"))
g++ "${OBJ_FILES[@]}" -o "${EXECUTABLE}"
echo "Linked object files into executable: ${EXECUTABLE}"

# cd testCases

# for file in $(ls | grep -E '^test6+$'); do
#     echo "Trying $file"
#     ../test "$file" 
    
# done

# cd ..


