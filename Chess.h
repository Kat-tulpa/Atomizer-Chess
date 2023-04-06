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
#include "FEN.hpp"
#include "CSV.hpp"

namespace Chess {

    

};