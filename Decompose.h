#pragma once

namespace Decompose {

    enum Shape {
        TRIANGLE,
        QUADRILATERAL,
        ELLIPSE,
    };

    static void board() {
        constexpr unsigned int sideLength = 8;
        grid(sideLength);
    }

    static void grid(const int sideLength) {
        std::vector<std::string> sequences;

    }

    static void allSubquadrilaterals(std::vector<std::string> sequenceList, const std::string& sequence, const unsigned int sideLength) {
        for (int x1 = 0; x1 < sideLength; x1++)
            for (int y1 = 0; y1 < sideLength; y1++)
                for (int x2 = x1 + 1; x2 <= sideLength; x2++)
                    for (int y2 = y1 + 1; y2 <= sideLength; y2++) {
                        const int width = x2 - x1;
                        const int height = y2 - y1;
                        if (width == height) {
                            const std::string subquad = sequence.substr(x1 * sideLength + y1, width * height);
                            sequenceList.push_back(subquad);
                        }
                    }
    }
};