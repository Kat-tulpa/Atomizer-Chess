#pragma once
#include <vector>
#include <string>
#include <utility>
#include <cmath>

#include "SequenceManager.h"

class Decompose {
private:
    static std::vector<uint8_t> sequenceManagerRegistrationIDs;

    enum class ShapeType : uint8_t {
        QUADRILATERAL,
        ELLIPSE,
        TYPECOUNT
    };

public:

    // Register Shape Categories With Sequence Manager
    static void init() {
        for (unsigned int i = 0; i < static_cast<unsigned int>(ShapeType::TYPECOUNT); i++)
            sequenceManagerRegistrationIDs.push_back(SequenceManager::makeAddDataType());
    }

    static void AllSubquadrilaterals(std::vector<std::pair<std::string, ShapeType>>& quadrilateral_list, const std::string& board, const uint8_t width) {
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

    static void AllSubellipses(std::vector<std::pair<std::string, ShapeType>>& ellipse_list, const std::string& board, const uint8_t width, const uint8_t height) {
        for (int x = 0; x < height; ++x) {
            for (int y = 0; y < width; ++y) {
                const int width_minus_y = width - y;
                const int height_minus_x = height - x;
                const int a_squared = height_minus_x * height_minus_x;
                bool should_break_a = false;
                for (int a = 1; a <= height_minus_x; ++a) {
                    const int a_squared_times_width = a_squared * width;
                    const int a_squared_times_width_plus_b_squared_times_width_minus_y = a_squared_times_width + (width_minus_y * width_minus_y) * a;
                    if (a_squared_times_width > a_squared_times_width_plus_b_squared_times_width_minus_y) {
                        should_break_a = true;
                        break;
                    }
                    const int a_squared_times_b_squared = a * a * width_minus_y * width_minus_y;
                    bool should_break_b = false;
                    for (int b = 1; b <= width_minus_y; ++b) {
                        if ((x + a > height) || (y + b > width)) {
                            should_break_b = true;
                            break;
                        }
                        const int b_squared_times_height_minus_x = b * b * height_minus_x * height_minus_x;
                        if (a_squared_times_b_squared >= b_squared_times_height_minus_x + a_squared * (width_minus_y * width_minus_y)) {
                            should_break_b = true;
                            break;
                        }
                        std::string subellipse = "";
                        const int a_squared_times_b_squared = a * a * b * b;
                        for (int i = x; i < x + a; ++i) {
                            if ((i < 0) || (i >= height)) {
                                continue;
                            }
                            for (int j = y; j < y + b; ++j) {
                                if ((j < 0) || (j >= width)) {
                                    continue;
                                }
                                if (((i - x) * (i - x) * b * b + (j - y) * (j - y) * a * a) <= a_squared_times_b_squared) {
                                    subellipse += board[i * width + j];
                                }
                            }
                        }
                        ellipse_list.emplace_back(subellipse, ShapeType::ELLIPSE);
                        if (should_break_b) {
                            break;
                        }
                    }
                    if (should_break_a) {
                        break;
                    }
                }
            }
        }
    }

    static void Grid(const std::string& grid_str) {
        std::vector<std::pair<std::string, ShapeType>> decomposed_shapes;
        const uint8_t width = std::sqrt(grid_str.length());
        AllSubquadrilaterals(decomposed_shapes, grid_str, width);
        AllSubellipses(decomposed_shapes, grid_str, width, width);
        addListToSequenceManager(decomposed_shapes);
    }

    static void addListToSequenceManager(std::vector<std::pair<std::string, ShapeType>>& shapeList) {
        for (unsigned int i = 0; i < shapeList.size(); i++)
            SequenceManager::addDataEntry(sequenceManagerRegistrationIDs[static_cast<uint8_t>(shapeList.at(i).second)], shapeList.at(i).first);
    }
};

std::vector<uint8_t> Decompose::sequenceManagerRegistrationIDs;