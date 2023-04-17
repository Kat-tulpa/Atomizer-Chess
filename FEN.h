#pragma once
#include <algorithm>
#include <string>
#include <vector>

#include "Geometric Decomposition.h"
#include "Utility.h"

class FEN {

public:

    enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
    enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

    static const GeometricDecomposition::Shape formatToShape(const std::string& fen) {
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

        return GeometricDecomposition::Shape{
            GeometricDecomposition::Shape::Type::RECTANGLE_SQUARE,
            GeometricDecomposition::Shape::Dimensions{8, 8},
            GeometricDecomposition::Shape::Offset{8, 8},
            Utility::stringToVector(board)
        };
    }
};