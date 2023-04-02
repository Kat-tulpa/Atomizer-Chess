#include <bitset>
#include "SequenceManager.h"

namespace Chess {

    enum Piece {
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

    namespace Piece {


        // Determines how many piece types are defined in the enum Piece for flexability
        static constexpr size_t TYPES = sizeof(Piece) / sizeof(Piece::EMPTY);
    };

    class Sequence {
    public:
        ChessSequence() = default;

        ChessSequence(size_t length) {
            data = 0ULL;
            this->length = length;
        }

        void setPiece(size_t index, Piece piece) {
            size_t shift = index * 4;
            data &= ~(0xFULL << shift);
            data |= static_cast<uint64_t>(piece) << shift;
        }

        Piece getPiece(size_t index) const {
            size_t shift = index * 4;
            return static_cast<Piece>((data >> shift) & 0xFULL);
        }

        std::string toString() {
            std::string result;
            for (size_t i = 0; i < getLength(); i++) {
                result += "-PNBRQKpnbrqk"[getPiece(i)];
            }
            return result;
        }

        size_t getLength() {
            return length;
        }

        void randomize() {
            for (size_t i = 0; i < this->length; i++) {
                Piece piece = static_cast<Piece>(std::rand() % PIECE_TYPES);
                setPiece(i, piece);
            }
        }

    private:
        uint64_t data;
        size_t length;
    };

    namespace Manager {
        static void addRandom(size_t count, size_t length) {
            for (size_t i = 0; i < count; i++) {
                Sequence sequence(length);
                sequence.randomize();
                SequenceManager::add(sequence.toString());
            }
        }
    };
};