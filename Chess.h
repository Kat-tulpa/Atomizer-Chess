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

    const uint8_t sequenceManagerTypeID = SequenceManager::makeAddDataType();

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
                    SequenceManager::addDataEntry(sequenceManagerTypeID, toSerializedData(fenString, evalScore));
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