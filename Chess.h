#include <bitset>
#include <fstream>

#include "SequenceManager.h"

namespace Chess {
    namespace Piece {
        enum TYPE {
            EMPTY,
            WHITE_KING,
            WHITE_QUEEN,
            WHITE_ROOK,
            WHITE_BISHOP,
            WHITE_KNIGHT,
            WHITE_PAWN,
            BLACK_KING,
            BLACK_QUEEN,
            BLACK_ROOK,
            BLACK_BISHOP,
            BLACK_KNIGHT,
            BLACK_PAWN,
            COUNT
        };
    };

    class Sequence {
    public:

    private:
        uint64_t data;
        size_t length;
    };

    // This code generates a board string from a FEN string by parsing the piece positions and the blank spaces on the board.
    std::string fenToBoardString(const std::string& fen) {
        size_t endOfBoard = fen.find(" ");
        std::string board = fen.substr(0, endOfBoard);

        std::string boardString;
        for (auto c : board) {
            if (isdigit(c)) {
                int numSpaces = c - '0';
                boardString.append(numSpaces, '-');
            }
            else if (c != '/') {
                boardString += c;
            }
        }
        boardString.append(64 - boardString.length(), '-');

        return boardString;
    }

    // This code parses a signed integer from a string that includes "+, -, and #" chess notation to indicate engine evaluation score
    static int parseSignedInt(const std::string& str) {
        if (isdigit(str[0])) return 0;

        unsigned int sign = 0;
        if (str[0] == '+')
            sign = 1;
        else if (str[0] == '-')
            sign = -1;
        else if (str[0] == '#')
            if (str[1] == '+')
                return 9999;
            else return -9999;

        int score = 0;
        for (unsigned int i = 1; i < str.length(); i++)
            score = score * 10 + (str[i] - '0');
        return sign * score;
    }

    static void load_FEN_CSV(const std::string& filename) {
        std::ifstream file(filename);
        if (file) {
            std::string line;
            while (std::getline(file, line)) {
                std::string field1, field2;
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    field1 = line.substr(0, pos);
                    field2 = line.substr(pos + 1);

                    // Add the sequence to SequenceManager
                    SequenceManager::add(fenToBoardString(field1), parseSignedInt(field2));
                }
            }
            file.close();
        }
    }
};