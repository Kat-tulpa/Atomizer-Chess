/*
About Chess.h
    Intention is to provide various utility functions and data structures related to chess, such as
    - Parsing FEN std::strings
    - Representing chess pieces
    - Loading FEN std::strings and evaluations from a CSV file into a SequenceManager
 */

/*
    Here is a summary of the main components of the code:

*/

#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "SequenceManager.h"
#include "Decompose.h"

namespace Chess {

    class FEN {
        enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
        enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
        enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

    public:

        static std::string& boardToString(const std::vector<char>& board) {
            std::string result;
            for (char c : board) {
                result.push_back(c);
            }
            return result;
        }

        static std::string boardFromFEN(const std::string& fen) {
            std::string board(64, ' ');

            int rank = 7, file = 0;
            for (char c : fen) {
                if (c == '/') {
                    rank--;
                    file = 0;
                }
                else if (isdigit(c)) {
                    file += c - '0';
                }
                else {
                    board[rank * 8 + file] = c;
                    file++;
                }
            }

            // Replace numbers with spaces
            std::transform(board.begin(), board.end(), board.begin(),
                [](char c) { return (isdigit(c) ? ' ' : c); });

            return Decompose::prefixParseData(board, 8, 8, 0, 0);
        }
    };

    class CSV {
    public:
        // Parses a signed integer from a std::string that includes "+, -, and #" chess notation to indicate engine evaluation score
        int parseEvalScore(std::string evalscore) {
            if (evalscore[0] == '#') { // Checkmate in X moves

                if (evalscore[1] == '+') {
                    return 9999 - std::stoi(evalscore.substr(2));
                }
                else if (evalscore[1] == '-') {
                    return -9999 + std::stoi(evalscore.substr(2));
                }
                else {
                    throw std::invalid_argument("Invalid evalscore format");
                }
            }
            else {
                return std::stoi(evalscore); // Regular Score
            }
        }

        static char* stringToCharArray(const std::string& str) {
            char* charArray = new char[str.size() + 1];
            std::copy_n(str.begin(), str.size(), charArray);
            charArray[str.size()] = '\0'; // null-terminate the char array
            return charArray;
        }

        static void loadCsvFile(std::string filename) {
            Decompose::init();

            const int MAX_LINE_LENGTH = 256;
            std::string line;
            line.reserve(MAX_LINE_LENGTH);
            unsigned int iterations = 0;

            std::ifstream file(filename);
            if (!file) {
                throw std::runtime_error("Error opening file");
            }

            while (std::getline(file, line)) {
                const std::string& fen = line.substr(0, line.find(' '));
                const std::string& boardString = FEN::boardFromFEN(stringToCharArray(fen));
                Decompose::Grid(boardString);
                if (++iterations % 100 == 0) {
                    break;
                }
            }

            file.close();
        }
    };
};