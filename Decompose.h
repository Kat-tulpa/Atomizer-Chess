#pragma once
#include <vector>
#include <string>
#include <utility>
#include <cmath>

#include "SequenceManager.h"


class Decompose {
private:
    //                     Shape Type | Origin X | Origin Y
    static std::vector<uint8_t> sequenceManagerRegistrationIDs[8][8]; // 2D array of vectors 

public:

    enum class ShapeType : uint8_t {
        QUADRILATERAL,
        ELLIPSE,
        TYPECOUNT
    };

    // Register Shape Categories With Sequence Manager
    static void init() {
        sequenceManagerRegistrationIDs;
        for (unsigned int x = 0; x < 8; x++)
            for (unsigned int y = 0; y < 8; y++) 
                for (unsigned int i = 0; i < static_cast<unsigned int>(ShapeType::TYPECOUNT); i++) {
                    sequenceManagerRegistrationIDs[x][y].push_back(SequenceManager::makeAddDataType());
                }
    }

    static std::string prefixParseData(std::string data, uint8_t width, uint8_t height, uint8_t origin_x, uint8_t origin_y) {
        return std::string(1, width) + std::string(1, height) + std::string(1, origin_x) + std::string(1, origin_y) + data;
    }

    static void AllSubquadrilaterals(std::vector<std::pair<uint8_t, std::string>>& quadrilateral_list,
        const std::string& board, const uint8_t width, const uint8_t height, const uint8_t origin_x, const uint8_t origin_y) {
        for (int size = std::min(width, height); size >= 1; --size) {
            for (int x1 = width - size; x1 >= 0; --x1) {
                for (int y1 = height - size; y1 >= 0; --y1) {
                    std::string subquad = board.substr(x1 * height + y1, size * size);
                    const std::pair p = std::make_pair(sequenceManagerRegistrationIDs[origin_x][origin_y][static_cast<uint8_t>(ShapeType::QUADRILATERAL)], subquad);
                    quadrilateral_list.emplace_back(p);
                }
            }
        }
    }

    static void AllSubellipses(std::vector<std::pair<uint8_t, std::string>>& ellipse_list,
        const std::string& board, const uint8_t width, const uint8_t height, const uint8_t origin_x, const uint8_t origin_y) {
        for (int x = 0; x < height; ++x) {
            for (int y = 0; y < width; ++y) {
                const int width_minus_y = width - y, height_minus_x = height - x, a_squared = height_minus_x * height_minus_x;
                bool should_break_a = false;
                for (int a = 1; a <= height_minus_x; ++a) {
                    const int a_squared_times_width = a_squared * width, a_squared_times_width_plus_b_squared_times_width_minus_y = a_squared_times_width + (width_minus_y * width_minus_y) * a;
                    if (a_squared_times_width > a_squared_times_width_plus_b_squared_times_width_minus_y) { should_break_a = true; break; }
                    const int a_squared_times_b_squared = a * a * width_minus_y * width_minus_y;
                    if ((x + a > height) || (y + 1 > width)) { should_break_a = true; break; }
                    bool should_break_b = false;
                    for (int b = 1; b <= width_minus_y; ++b) {
                        const int b_squared_times_height_minus_x = b * b * height_minus_x * height_minus_x;
                        if (a_squared_times_b_squared >= b_squared_times_height_minus_x + a_squared * (width_minus_y * width_minus_y)) { should_break_b = true; break; }
                        std::string subellipse = "";
                        const int a_squared_times_b_squared = a * a * b * b;
                        for (int i = x; i < x + a; ++i) {
                            if ((i < 0) || (i >= height)) { continue; }
                            for (int j = y; j < y + b; ++j) {
                                if ((j < 0) || (j >= width)) { continue; }
                                if (((i - x) * (i - x) * b * b + (j - y) * (j - y) * a * a) <= a_squared_times_b_squared) {
                                    subellipse += board[i * width + j];
                                }
                            }
                        }
                        const std::pair p = std::make_pair(static_cast<uint8_t>(ShapeType::ELLIPSE), prefixParseData(subellipse, width, height, origin_x, origin_y));
                        ellipse_list.emplace_back(p);
                        if (should_break_b) { break; }
                    }
                    if (should_break_a) { break; }
                }
            }
        }
    }

    static void Grid(std::string grid_str) {
        std::vector<std::pair<uint8_t, std::string>> decomposed_shapes;
        const uint8_t width = grid_str[0];
        const uint8_t height = grid_str[1];
        const uint8_t origin_x = grid_str[2];
        const uint8_t origin_y = grid_str[3];
        grid_str.erase(0, 4); // delete the first 4 characters

        AllSubquadrilaterals(decomposed_shapes, grid_str, width, height, origin_x, origin_y);
        //AllSubellipses(decomposed_shapes, grid_str, width, height, origin_x, origin_y);
        addListToSequenceManager(decomposed_shapes);
    }

    static void addListToSequenceManager(std::vector<std::pair<uint8_t, std::string>>& shapeList) {
        for (unsigned int i = 0; i < shapeList.size(); i++)
            SequenceManager::addDataEntry(shapeList[i].first, shapeList[i].second);
    }
};

std::vector<uint8_t> Decompose::sequenceManagerRegistrationIDs[8][8];