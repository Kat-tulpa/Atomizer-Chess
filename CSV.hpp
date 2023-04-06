#pragma once

#include <stdexcept>
#include <fstream>
#include "SequenceManager.h"

const uint8_t sequenceManagerTypeID = SequenceManager::makeAddDataType();

// This code parses a signed integer from a string that includes "+, -, and #" chess notation to indicate engine evaluation score
int16_t scoreStringToInt(const std::string& str) {
    if (isdigit(str[0])) return 0;

    unsigned int sign = 0;
    if (str[0] == '+')
        sign = 1;
    else if (str[0] == '-')
        sign = -1;
    else if (str[0] == '#')
        if (str[1] == '+')
            return 9999;
        else return -9999;

    int score = 0;
    for (unsigned int i = 1; i < str.length(); i++)
        score = score * 10 + (str[i] - '0');
    return sign * score;
}

// Opens the file with the given filename for reading, throwing an exception if it cannot be opened
static void open(std::ifstream& file, const std::string& filename) {
    file.open(filename);
    if (!file) throw std::runtime_error("Unable to open file");
}

// Returns the next line from input stream as a string, or an empty string if end of file is reached
static std::string readNextLine(std::istream& input) {
    std::string line;
    if (std::getline(input, line))
        return line;
    else
        return "";
}

void parseLine(const std::string& line) {
    size_t pos = line.find(',');
    if (pos != std::string::npos) {
        const std::string fenString = line.substr(0, pos);
        const std::string evalString = line.substr(pos + 1);
        const int16_t evalScore = scoreStringToInt(evalString);
        SequenceManager::addDataEntry(sequenceManagerTypeID, toSerializedData(fenString, evalScore));
    }
    else {
        throw std::runtime_error("Invalid CSV format");
    }
}

// Loads FEN strings and evaluations from a CSV file into SequenceManager
static void load(const std::string& filename) {
    std::ifstream file;
    open(file, filename);
    std::string line;
    while ((line = readNextLine(file)) != "")
        parseLine(line);
    file.close();
}