#pragma once
#include <vector>
#include <string>
#include <utility>
#include <cmath>

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace Decompose {
    enum class ShapeType : uint8_t {
        QUADRILATERAL,
        ELLIPSE
    };

    void AllSubquadrilaterals(std::vector<std::pair<std::string, ShapeType>>& quadrilateral_list, const std::string& board, const uint8_t width) {
        const uint8_t height = board.length() / width;
        for (int x1 = 0; x1 < width; ++x1) {
            for (int y1 = 0; y1 < height; ++y1) {
                for (int size = 1; size <= width - x1 && size <= height - y1; ++size) {
                    if (size == width - x1 || size == height - y1 || board[(x1 + size) * height + y1 + size - 1] == '.') {
                        const std::string subquad = board.substr(x1 * height + y1, size * size);
                        quadrilateral_list.emplace_back(subquad, ShapeType::QUADRILATERAL);
                    }
                }
            }
        }
    }

    void AllSubellipses(std::vector<std::pair<std::string, ShapeType>>& ellipse_list, const std::string& board, const uint8_t width) {
        const uint8_t height = board.length() / width;
        for (int x = 0; x < height; ++x) {
            for (int y = 0; y < width; ++y) {
                for (int a = 1; a <= height - x; ++a) {
                    if ((a * a * width) > ((height - x) * (height - x) * width + (width - y) * (width - y) * a)) {
                        break;
                    }
                    for (int b = 1; b <= width - y; ++b) {
                        if (((x + a <= height) && (y + b <= width)) && (a * a * b * b >= (b * b * (height - x) * (height - x) + a * a * (width - y) * (width - y)))) {
                            break;
                        }
                        std::string subellipse = "";
                        for (int i = x; i < x + a; ++i) {
                            for (int j = y; j < y + b; ++j) {
                                if (((i - x) * (i - x) * b * b + (j - y) * (j - y) * a * a) <= a * a * b * b) {
                                    subellipse += board[i * width + j];
                                }
                            }
                        }
                        ellipse_list.emplace_back(subellipse, ShapeType::ELLIPSE);
                    }
                }
            }
        }
    }

    std::vector<std::pair<std::string, ShapeType>> Grid(const std::string& grid_str) {
        std::vector<std::pair<std::string, ShapeType>> decomposed_shapes;
        const uint8_t width = std::sqrt(grid_str.length());
        AllSubquadrilaterals(decomposed_shapes, grid_str, width);
        AllSubellipses(decomposed_shapes, grid_str, width);
        return decomposed_shapes;
    }
}