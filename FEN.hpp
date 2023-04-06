#pragma once

#include <string>
#include <cstdint>
#include "SerializedData.h"
#include "PieceType.hpp"

extern std::array<PIECETYPE, 256> pieceLookup;

SerializedData toSerializedData(const std::string& fen, const int16_t extraData) {
    // Create an empty board
    PIECETYPE board[64] = { PIECETYPE::EMPTY };

    // Parse the FEN string and fill in the board
    int i = 0, j = 0;
    for (auto c : fen) {
        if (c == ' ') break; // Stop parsing at end of board section
        if (c == '/') {
            i++;
            j = 0;
        }
        else if (c >= '1' && c <= '8') {
            j += c - '0';
        }
        else {
            PIECETYPE pt = pieceLookup[c];
            board[i * 8 + j] = pt;
            j++;
        }
    }

    // Pack the board into a string
    std::string packedData;
    for (int i = 0; i < 64; i++) {
        packedData += std::to_string(static_cast<int>(board[i]));
    }

    // Pack the extraData value into bits and add it to the packedData string
    packedData += std::bitset<16>(extraData).to_string();

    SerializedData data;
    data.declareBitLength(packedData.length()); // Give size in bits
    data.setData(packedData);
    return data;