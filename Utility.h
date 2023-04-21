#pragma once
#include <string>
#include <vector>
#include <stdexcept>

class Utility {
public:
    static char* stringToCharArray(const std::string& str) {
        char* charArray = new char[str.size() + 1];
        std::copy_n(str.begin(), str.size(), charArray);
        charArray[str.size()] = '\0'; // null-terminate the char array
        return charArray;
    }

    static std::vector<char> stringToVector(const std::string& str) {
        return std::vector<char>(str.begin(), str.end());
    }

    // Parses a signed integer from a std::string that includes "+, -, and #" chess notation to indicate engine evaluation score
    static int parseEvalScore(std::string evalscore) {
        if (evalscore[0] == '#') { // Checkmate in X moves

            if (evalscore[1] == '+') {
                return 9999 - std::stoi(evalscore.substr(2));
            }
            else if (evalscore[1] == '-') {
                return -9999 + std::stoi(evalscore.substr(2));
            }
            else {
                throw std::invalid_argument("Invalid evalscore format");
            }
        }
        else {
            return std::stoi(evalscore); // Regular Score
        }
    }
};