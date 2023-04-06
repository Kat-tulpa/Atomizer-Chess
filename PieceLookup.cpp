#pragma once

#include "PieceType.hpp"
#include <array>

constexpr std::array<PIECETYPE, 256> initPieceLookup()
{
    std::array<PIECETYPE, 256> pieceLookup{ PIECETYPE::EMPTY };
    pieceLookup[static_cast<uint8_t>('P')] = PIECETYPE::WHITE_PAWN;
    pieceLookup[static_cast<uint8_t>('N')] = PIECETYPE::WHITE_KNIGHT;
    pieceLookup[static_cast<uint8_t>('B')] = PIECETYPE::WHITE_BISHOP;
    pieceLookup[static_cast<uint8_t>('R')] = PIECETYPE::WHITE_ROOK;
    pieceLookup[static_cast<uint8_t>('Q')] = PIECETYPE::WHITE_QUEEN;
    pieceLookup[static_cast<uint8_t>('K')] = PIECETYPE::WHITE_KING;
    pieceLookup[static_cast<uint8_t>('p')] = PIECETYPE::BLACK_PAWN;
    pieceLookup[static_cast<uint8_t>('n')] = PIECETYPE::BLACK_PAWN;
    pieceLookup[static_cast<uint8_t>('b')] = PIECETYPE::BLACK_PAWN;
    pieceLookup[static_cast<uint8_t>('r')] = PIECETYPE::BLACK_PAWN;
    pieceLookup[static_cast<uint8_t>('q')] = PIECETYPE::BLACK_PAWN;
    pieceLookup[static_cast<uint8_t>('k')] = PIECETYPE::BLACK_PAWN;
    return pieceLookup;
}

constexpr std::array<PIECETYPE, 256> pieceLookup = initPieceLookup();