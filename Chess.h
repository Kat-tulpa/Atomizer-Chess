/*
About Chess.h
    Intention is to provide various utility functions and data structures related to chess, such as
    - Parsing FEN strings
    - Representing chess pieces
    - Loading FEN strings and evaluations from a CSV file into a SequenceManager
 */

/*
    Here is a summary of the main components of the code:

    Piece namespace: 
        - contains an enum representing the different types of chess pieces, 
        - as well as a utility function for converting characters to piece types

    fenToBoardString function: 
        - takes a FEN string as input and returns a string representing the initial state of a chess board, 
        - with pieces represented by characters and empty spaces represented by dashes

    parseSignedInt function: 
        - takes a string representing in chess engine evaluation score notation (with +, -, or # as prefixes)
        - and returns the corresponding integer value.

    openFile function: 
        - takes a filename and an ifstream object as inputs, 
        - attempts to open the file for reading, 
        - and throws an exception if the file cannot be opened

    readNextLine function: 
        - takes an input stream as input and returns the next line from the stream as a string, 
        - or an empty string if the end of the stream is reached

    parseFenAndEvalStrings function: 
        - takes a line from a CSV file as input and parses it into separate FEN and evaluation strings, returning them as a pair

    loadFenAndEvalFromLine function: 
        - takes a line from a CSV file as input, 
        - uses fenToBoardString and parseSignedInt to convert the FEN and evaluation strings to the appropriate formats, 
        - and adds them to a SequenceManager.

    load_FEN_CSV function: 
        - takes a filename as input, 
        - opens the corresponding file, 
        - reads each line of the file, 
        - and calls loadFenAndEvalFromLine on each line to load the FEN strings and evaluations into a SequenceManager
*/

#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include "SequenceManager.h"

namespace Chess {
    class PieceLookup {
    public:
        enum class PIECETYPE : uint8_t {
            EMPTY,
            WHITE_PAWN,
            WHITE_KNIGHT,
            WHITE_BISHOP,
            WHITE_ROOK,
            WHITE_QUEEN,
            WHITE_KING,
            BLACK_PAWN,
            BLACK_KNIGHT,
            BLACK_BISHOP,
            BLACK_ROOK,
            BLACK_QUEEN,
            BLACK_KING,
            COUNT
        };
        static PIECETYPE table[256];
        static void init();
    };
    PieceLookup::PIECETYPE PieceLookup::table[256];
    void PieceLookup::init() {
        table[static_cast<uint8_t>('P')] = PIECETYPE::WHITE_PAWN;
        table[static_cast<uint8_t>('N')] = PIECETYPE::WHITE_KNIGHT;
        table[static_cast<uint8_t>('B')] = PIECETYPE::WHITE_BISHOP;
        table[static_cast<uint8_t>('R')] = PIECETYPE::WHITE_ROOK;
        table[static_cast<uint8_t>('Q')] = PIECETYPE::WHITE_QUEEN;
        table[static_cast<uint8_t>('K')] = PIECETYPE::WHITE_KING;
        table[static_cast<uint8_t>('p')] = PIECETYPE::BLACK_PAWN;
        table[static_cast<uint8_t>('n')] = PIECETYPE::BLACK_KNIGHT;
        table[static_cast<uint8_t>('b')] = PIECETYPE::BLACK_BISHOP;
        table[static_cast<uint8_t>('r')] = PIECETYPE::BLACK_ROOK;
        table[static_cast<uint8_t>('q')] = PIECETYPE::BLACK_QUEEN;
        table[static_cast<uint8_t>('k')] = PIECETYPE::BLACK_KING;
    }

    class PackedBoard {
    public:
        static uint8_t sequenceManagerTypeID;
        static uint8_t getNextSequenceManagerTypeID();
        static void init();
    };
    uint8_t PackedBoard::sequenceManagerTypeID;
    uint8_t PackedBoard::getNextSequenceManagerTypeID() {
        return SequenceManager::makeAddDataType();
    }
    void PackedBoard::init() {
        sequenceManagerTypeID = getNextSequenceManagerTypeID();
    }

    class FEN {
    public:
        static std::string parseFen(const std::string& fen);
        static std::string packBoard(const std::string& board);
    };

    std::string FEN::parseFen(const std::string& fen) {
        std::string board(64, ' ');
        int boardIndex = 0;
        for (char c : fen) {
            if (c == ' ') {
                break; // End of piece placement section
            }
            else if (c == '/') {
                continue; // End of rank
            }
            else if (std::isdigit(c)) {
                boardIndex += c - '0';
            }
            else {
                board[boardIndex++] = c;
            }
        }
        return board;
    }

    std::string FEN::packBoard(const std::string& board) {
        std::string packedBoard;
        packedBoard.reserve(32);

        for (int i = 0; i < 64; ++i) {
            int packedIndex = i / 2;
            char pieceChar = board[i];
            Chess::PieceLookup::PIECETYPE pieceType = Chess::PieceLookup::table[static_cast<uint8_t>(pieceChar)];

            if (i % 2 == 0) {
                packedBoard.push_back(static_cast<char>(static_cast<int>(pieceType) << 4));
            }
            else {
                packedBoard[packedIndex] |= static_cast<char>(static_cast<int>(pieceType) & 0x0F);
            }
        }

        return packedBoard;
    }

    class CSV {
    public:
        // Parses a signed integer from a string that includes "+, -, and #" chess notation to indicate engine evaluation score
        static int16_t scoreStringToInt(const std::string& str) {
            int sign = 0;
            if (str[0] == '+')
                sign = 1;
            else if (str[0] == '-')
                sign = -1;

            int16_t result = 0;
            size_t i = sign != 0 ? 1 : 0;

            for (; i < str.size(); ++i) {
                if (str[i] == '#') {
                    result = 32000;
                    break;
                }
                result = result * 10 + (str[i] - '0');
            }

            if (sign == -1) result = -result;
            return result;
        }

        // Parses a CSV line and returns a vector of strings
        static std::vector<std::string> parseCSVLine(const std::string& line) {
            std::vector<std::string> result;

            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, ',')) {
                result.push_back(cell);
            }

            return result;
        }

        static void loadCsvFile(const std::string& filename) {
            PieceLookup::init();
            PackedBoard::init();

            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Failed to open file " << filename << std::endl;
                return;
            }

            const int MAX_LINE_LENGTH = 256;
            std::string line;
            line.reserve(MAX_LINE_LENGTH);
            while (std::getline(file, line)) {
                const auto pos = line.find(',');
                if (pos == std::string::npos) {
                    continue; // invalid line format, skip it
                }
                const std::string& fenString = FEN::parseFen(line.substr(0, pos));
                const std::string& parsedFen = FEN::packBoard(fenString);
                SequenceManager::addDataEntry(PackedBoard::sequenceManagerTypeID, parsedFen);
            }

            file.close();
        }
    };
};