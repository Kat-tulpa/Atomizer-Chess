#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "ShapeFeature.hpp"
#include "Utility.hpp"

class FEN {

public:

    static const std::vector<char> positionStringToCharSequence(const std::string& fen) {
        std::string board(64, ' ');

        int rank = 7, file = 0;
        for (char c : fen) {
            if (c == '/') {
                rank--;
                file = 0;
            }
            else if (isdigit(c)) {
                file += c - '0';
            }
            else {
                board.at(rank * int(8) + file) = c;
                file++;
            }
        }

        // Replace numbers with spaces
        std::transform(board.begin(), board.end(), board.begin(),
            [](char c) { return (isdigit(c) ? ' ' : c); });

        return Utility::stringToCharVector(board);
    }

    static float evalStringToFloat(const std::string& s) {
        switch (s[0]) {
        case '+':
            return std::stoi(s.substr(1));
        case '-':
            return -std::stoi(s.substr(1));
        case '#':
            if (s[1] == '+')
                return 1000.f; 
            return -1000.f;
        }
        return 0.f;
    }

    // Parses a signed integer from a std::string that includes "+, -, and #" chess notation to indicate engine evaluation score
    static float parseEvalScore(std::string evalscore) {
        if (evalscore[0] == '#') { // Checkmate in X moves

            if (evalscore[1] == '+') {
                return 9999.f;
            }
            else if (evalscore[1] == '-') {
                return -9999.f;
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