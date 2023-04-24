#pragma once
#include <algorithm>
#include <string>
#include <vector>

#include "Shape.h"
#include "Utility.h"

class FEN {

public:

    static const Shape positionStringToShape(const std::string& fen) {
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

        return Shape{
            Shape::Type::RECTANGLE_SQUARE,
            Shape::Dimensions{8, 8},
            Shape::Offset{8, 8},
            Utility::stringToVector(board)
        };
    }

    static float evalStringToFloat(const std::string& s) {
        int value = 0;
        bool isPositive = true;

        switch (s[0]) {
        case '+':
            value = std::stoi(s.substr(1));
            break;
        case '-':
            value = std::stoi(s);
            isPositive = false;
            break;
        case '#':
            value = (s[1] == '+') ? 5000 : -5000;
            isPositive = (s[1] == '+');
            break;
        default:
            value = std::stoi(s);
            break;
        }

        return isPositive ? static_cast<float>(value) : static_cast<float>(-value);
    }
};