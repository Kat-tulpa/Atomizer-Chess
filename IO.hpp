#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <iomanip> // For setprecision
#include "Defs.hpp"

namespace IO {
    using namespace Chess;
    static bool isPieceChar(const char c) {
        return LookupTables::IS_PIECE_NOTATION(static_cast<unsigned char>(c));
    }

    static const std::string ansi_color(int R, int G, int B) {
        std::string s = "\033[38;2;" 
            + std::to_string(R) + ";" 
            + std::to_string(G) + ";"
            + std::to_string(B) + "m";
        return s;
    }

    namespace Print {
        static void modelError(const float model_error) {
            std::cout << "Model Error: " << model_error << std::endl;
        }

        static void pieceWeightsAsBoard(const char piece_type, 
            const std::vector<float>& weights, 
            const std::vector<std::string>& reverse_map) {
            using namespace BoardProperties;

            float weights_2d[CHESS_BOARD_WIDTH][CHESS_BOARD_HEIGHT] = { 0.f };
            std::cout << std::setprecision(2);

            float max_value = 0.f;
            for (unsigned int i = 0; i < weights.size(); i++) {
                if (reverse_map[i][4] == piece_type) {
                    weights_2d[reverse_map[i][2] - '0']
                        [reverse_map[i][3] - '0'] = weights[i];
                    if (weights[i] > max_value)
                        max_value = weights[i];
                }
            }

            for (unsigned int y = 0; y < RANK_COUNT; y++) {
                for (unsigned int x = 0; x < FILE_COUNT; x++) {
                    const float color_scaled_to_weight 
                        = weights_2d[x][y] / max_value;
                    std::cout << "   " << 
                        ansi_color(color_scaled_to_weight, 150, 150) 
                        << weights_2d[x][y] << "   ";
                }

                std::cout << std::endl;
            }
        }
    };

    void writeFloatVectorToFile(const std::vector<float>& data, const std::string& filename) {
        std::ofstream outputFile(filename, std::ios::binary);
        if (outputFile.is_open()) {
            // Write the vector data to the file
            outputFile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));

            // Close the file
            outputFile.close();

            std::cout << "Vector data has been written to the file: " << filename << std::endl;
        }
        else {
            std::cout << "Unable to open the file: " << filename << std::endl;
        }
    }

    const std::pair<std::vector<float>, bool> readFloatVectorFromFile(const std::string& filename) {
        std::ifstream inputFile(filename, std::ios::binary);
        std::vector<float> data;
        bool success = true;

        if (inputFile.is_open()) {
            // Get the file size
            inputFile.seekg(0, std::ios::end);
            std::streampos fileSize = inputFile.tellg();
            inputFile.seekg(0, std::ios::beg);

            // Resize the vector to fit the data
            data.resize(fileSize / sizeof(float));

            // Read the data from the file into the vector
            inputFile.read(reinterpret_cast<char*>(data.data()), fileSize);

            // Close the file
            inputFile.close();

            std::cout << "Vector data has been read from the file: " << filename << std::endl;
        }
        else {
            std::cout << "Unable to open the file: " << filename << std::endl;
            success = false;
        }

        return std::make_pair(data, success);
    }
};