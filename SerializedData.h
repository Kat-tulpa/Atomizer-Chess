#pragma once
 
#include <cstdint>
#include <bitset>
#include <sstream>

struct SerializedData {
public:
    // Calculates the bit length of data based on the first 8 bits (0 - 254) of the input string 
    uint8_t getDataBitLength() const {
        std::bitset<8> bits(bitData.substr(0, 8));
        return static_cast<uint8_t>(bits.to_ulong());
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << static_cast<char>(bitLength) << bitData;
        return oss.str();
    }

    void print() const {
        std::cout << "Bit length: " << static_cast<int>(bitLength) << std::endl;
        std::cout << "Bit data: " << serialize() << std::endl;
    }

    void declareBitLength(const uint8_t& length) {
        bitLength = length;
    }

    void setData(const std::string& data) {
        bitData = data;
    }

private:
    uint8_t bitLength;
    std::string bitData;
};