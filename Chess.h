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

#include <bitset>
#include <fstream>
#include "SequenceManager.h"

namespace Chess {
    namespace Piece {
        static enum TYPE {
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

        static TYPE getFromChar(const char c) {
            static constexpr TYPE charToPiece[] = {
                EMPTY,          // '-'
                WHITE_KING,     // 'k'
                WHITE_QUEEN,    // 'q'
                WHITE_ROOK,     // 'r'
                WHITE_BISHOP,   // 'b'
                WHITE_KNIGHT,   // 'n'
                WHITE_PAWN,     // 'p'
                BLACK_KING,     // 'K'
                BLACK_QUEEN,    // 'Q'
                BLACK_ROOK,     // 'R'
                BLACK_BISHOP,   // 'B'
                BLACK_KNIGHT,   // 'N'
                BLACK_PAWN      // 'P'
            };

            if (c >= '-' && c <= 'P') {
                return charToPiece[c - '-'];
            }
            else
                throw std::invalid_argument("Invalid character");
        }
    }

    // This code generates a board string from a FEN string by parsing the piece positions and the blank spaces on the board
    static std::string fenToBoardString(const std::string& fen) {
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

    // Opens the file with the given filename for reading, throwing an exception if it cannot be opened
    static void openFile(std::ifstream& file, const std::string& filename) {
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

    // Parses a line from the CSV file into separate FEN and evaluation strings, returned as a std::pair
    static std::pair<std::string, std::string> parseFenAndEvalStrings(const std::string& line) {
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            std::string fenString = line.substr(0, pos);
            std::string evalString = line.substr(pos + 1);
            return std::make_pair(fenString, evalString);
        }
        else
            throw std::runtime_error("Invalid CSV format");
    }

    // Adds parsed FEN string and evaluation to SequenceManager
    static void loadFenAndEvalFromLine(const std::string& line) {
        auto [fenString, evalString] = parseFenAndEvalStrings(line);
        SequenceManager::add(fenToBoardString(fenString), parseSignedInt(evalString));
    }

    // Loads FEN strings and evaluations from a CSV file into SequenceManager
    static void load_FEN_CSV(const std::string& filename) {
        std::ifstream file;
        openFile(file, filename);
        std::string line;
        while ((line = readNextLine(file)) != "")
            loadFenAndEvalFromLine(line);
        file.close();
    }

};