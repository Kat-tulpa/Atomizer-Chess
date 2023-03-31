#pragma once
/*
static unsigned long long charArrayToULL(char* charArray, size_t len) {
    unsigned long long result = 0;
    for (size_t i = 0; i < len; i++) {
        if (charArray[i] >= '0' && charArray[i] <= '9') {
            result = result * 10 + (charArray[i] - '0');
        }
        else {
            std::cerr << "Invalid character in charArray: " << charArray[i] << std::endl;
            return 0;
        }
    }
    return result;
}

static const unsigned long long UINTArrayToULL(const unsigned int* uintArray, size_t len) {
    unsigned long long result = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned int uintVal = uintArray[i];
        // Get the number of digits in the unsigned int value
        size_t numDigits = 0;
        do {
            numDigits++;
            uintVal /= 10;
        } while (uintVal != 0);
        // Shift the existing value left by the number of digits in the unsigned int value and add the unsigned int value
        result = (result * pow(10, numDigits)) + uintArray[i];
    }
    return result;
}


*/