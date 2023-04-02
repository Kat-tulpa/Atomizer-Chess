#include <bitset>
#include "SequenceManager.h"

static constexpr char pieceNotation[] = "-PNBRQKpnbrqk";
static constexpr size_t pieceTypes = sizeof(pieceNotation) - 1;

// Define the 13 possible chess pieces
enum ChessPiece {
    EMPTY,
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_PAWN
};

// Use bitpacking to store a sequence of chess pieces
class ChessSequence {
public:
    ChessSequence() = default;

    ChessSequence(size_t length) {
        size_t numPieces = length;
        if (numPieces > 64)
            numPieces = 64;
        for (size_t i = 0; i < numPieces; i++)
            setPiece(i, EMPTY);
    }

    void setPiece(size_t index, ChessPiece piece) {
        size_t shift = index * 4;
        data &= ~(0xFULL << shift);
        data |= static_cast<uint64_t>(piece) << shift;
    }

    ChessPiece getPiece(size_t index) const {
        size_t shift = index * 4;
        return static_cast<ChessPiece>((data >> shift) & 0xFULL);
    }

    std::string toString() {
        std::string result;
        for (size_t i = 0; i < getLength(); i++) {
            result += pieceNotation[getPiece(i)];
        }
        return result;
    }

    size_t getLength() {
        return length;
    }

    void setLength(size_t _length) {
        length = _length;
    }

    void randomize() {
        for (size_t i = 0; i < this->length; i++) {
            ChessPiece piece = static_cast<ChessPiece>(std::rand() % pieceTypes);
            setPiece(i, piece);
        }
    }

private:
    uint64_t data;
    size_t length;
};

namespace ChessSequenceManager {
    // Create a random chess sequence and add it to the sequence manager
    static void addRandom(size_t count, size_t length) {
        for (size_t i = 0; i < count; i++) {
            ChessSequence sequence;
            sequence.setLength(length);
            sequence.randomize();
            std::string sequenceString = sequence.toString();
            SequenceManager::add(sequenceString);
        }
    }
};