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

#include <fstream>
#include "SequenceManager.h"

namespace Chess {
    enum PIECETYPE {
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

    const uint8_t sequenceManagerTypeID = SequenceManager::makeAddDataType();

    namespace FEN {
        enum class PIECETYPE : uint8_t {
            EMPTY = 0x00,
            PAWN = 0x01,
            KNIGHT = 0x02,
            BISHOP = 0x03,
            ROOK = 0x04,
            QUEEN = 0x05,
            KING = 0x06,
            BLACK = 0x08,
            WHITE = 0x10
        };

        PIECETYPE operator|(PIECETYPE a, PIECETYPE b)
        {
            return static_cast<PIECETYPE>(static_cast<int>(a) | static_cast<int>(b));
        }

        std::unordered_map<char, PIECETYPE> pieceLookup = {
            {'P', PIECETYPE::PAWN},
            {'N', PIECETYPE::KNIGHT},
            {'B', PIECETYPE::BISHOP},
            {'R', PIECETYPE::ROOK},
            {'Q', PIECETYPE::QUEEN},
            {'K', PIECETYPE::KING},
            {'p', PIECETYPE::PAWN | PIECETYPE::BLACK},
            {'n', PIECETYPE::KNIGHT | PIECETYPE::BLACK},
            {'b', PIECETYPE::BISHOP | PIECETYPE::BLACK},
            {'r', PIECETYPE::ROOK | PIECETYPE::BLACK},
            {'q', PIECETYPE::QUEEN | PIECETYPE::BLACK},
            {'k', PIECETYPE::KING | PIECETYPE::BLACK},
            {'1', PIECETYPE::EMPTY},
            {'2', PIECETYPE::EMPTY},
            {'3', PIECETYPE::EMPTY},
            {'4', PIECETYPE::EMPTY},
            {'5', PIECETYPE::EMPTY},
            {'6', PIECETYPE::EMPTY},
            {'7', PIECETYPE::EMPTY},
            {'8', PIECETYPE::EMPTY},
            {'W', PIECETYPE::PAWN | PIECETYPE::WHITE},
            {'X', PIECETYPE::KNIGHT | PIECETYPE::WHITE},
            {'Y', PIECETYPE::BISHOP | PIECETYPE::WHITE},
            {'Z', PIECETYPE::ROOK | PIECETYPE::WHITE},
            {'T', PIECETYPE::QUEEN | PIECETYPE::WHITE},
            {'U', PIECETYPE::KING | PIECETYPE::WHITE}
        };

        // This code generates a board string from a FEN string by parsing the piece positions and the blank spaces on the board
        static std::string toBoardString(const std::string& fen) {
            size_t endOfBoard = fen.find(" ");
            std::string board = fen.substr(0, endOfBoard);

            std::string boardString;
            for (auto c : board)
                if (isdigit(c)) {
                    int numSpaces = c - '0';
                    boardString.append(numSpaces, '-');
                }
                else if (c != '/')
                    boardString += c;

            boardString.append(64 - boardString.length(), '-');

            return boardString;
        }

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
        }

        namespace CSV {

            // This code parses a signed integer from a string that includes "+, -, and #" chess notation to indicate engine evaluation score
            int16_t scoreStringToInt(const std::string& str) {
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

            // Opens the file with the given filename for reading, throwing an exception if it cannot be opened
            static void open(std::ifstream& file, const std::string& filename) {
                file.open(filename);
                if (!file) throw std::runtime_error("Unable to open file");
            }

            // Returns the next line from input stream as a string, or an empty string if end of file is reached
            static std::string readNextLine(std::istream& input) {
                std::string line;
                if (std::getline(input, line))
                    return line;
                else
                    return "";
            }

            void parseLine(const std::string& line) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    const std::string fenString = line.substr(0, pos);
                    const std::string evalString = line.substr(pos + 1);
                    const int16_t evalScore = scoreStringToInt(evalString);
                    SequenceManager::addDataEntry(sequenceManagerTypeID, toSerializedData(toBoardString(fenString), evalScore));
                }
                else {
                    throw std::runtime_error("Invalid CSV format");
                }
            }

            // Loads FEN strings and evaluations from a CSV file into SequenceManager
            static void load(const std::string& filename) {
                std::ifstream file;
                open(file, filename);
                std::string line;
                while ((line = readNextLine(file)) != "")
                    parseLine(line);
                file.close();
            }
        };
    };
};