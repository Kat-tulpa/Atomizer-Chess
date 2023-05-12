#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include "GeometricProperties.hpp"

namespace Chess {
    static constexpr size_t ORIGINAL_BOARD_SAMPLE_SIZE = 10000;
	static constexpr size_t RANK_COUNT = 8;
	static constexpr size_t FILE_COUNT = 8;

    namespace LookupTables {
        static constexpr size_t SIGNED_CHAR_MAX 
            = std::numeric_limits<signed char>::max();
        static constexpr bool IS_PIECE_NOTATION(char c) {
            switch (c) {
            case 'P': return true;
            case 'p': return true;
            case 'K': return true;
            case 'k': return true;
            case 'N': return true;
            case 'n': return true;
            case 'Q': return true;
            case 'q': return true;
            case 'R': return true;
            case 'r': return true;
            case 'B': return true;
            case 'b': return true;
            default: return false;
            }
        }

        static constexpr char FILE_NUMBER_TO_LETTER[Chess::FILE_COUNT]
            = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };
    };

    namespace IO {
        static constexpr const char* const CSV_POSITION_EVALUATION_FILE_NAME 
            = "chessData.csv";
        static constexpr const char CSV_DELIMITERS[]{ ' ', ',' };
    };

    namespace BoardProperties {
        static constexpr GeometricProperties::ShapeType 
            CHESS_BOARD_SHAPE_TYPE = 
            GeometricProperties::ShapeType::RECTANGLE;

        static constexpr size_t CHESS_BOARD_WIDTH = 8;
        static constexpr size_t CHESS_BOARD_HEIGHT = 8;
        static const Dimensions CHESS_BOARD_DIMENSIONS{ 
            CHESS_BOARD_WIDTH, CHESS_BOARD_HEIGHT };

        static constexpr size_t CHESS_BOARD_OFFSET_X = 0;
        static constexpr size_t CHESS_BOARD_OFFSET_Y = 0;
        static const Offsets CHESS_BOARD_OFFSETS { 
            CHESS_BOARD_OFFSET_X, CHESS_BOARD_OFFSET_Y };

        static const GeometricProperties CHESS_BOARD_PROPERTIES {
            CHESS_BOARD_SHAPE_TYPE,
            CHESS_BOARD_DIMENSIONS,
            CHESS_BOARD_OFFSETS
        };
    }
};

namespace EvaluationModel {
    static constexpr size_t ERROR_MAX = std::numeric_limits<size_t>::max();
    static constexpr float WEIGHT_DEFAULT = 0.f;
    static constexpr float EVALUATION_MIN = -500.f;
    static constexpr float EVALUATION_MAX = 500.f;
    static constexpr float FREQUENCY_DEFAULT = 0.0107f;
    static constexpr float MAGNITUDE_DEFAULT = 0.5f;
    static constexpr size_t ROUNDS_DEFAULT = 100000;
};