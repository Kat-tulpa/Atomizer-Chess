#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class CSVReader {
public:
    CSVReader(const std::string& filename) {
        m_file.open(filename);
        if (!m_file.is_open()) {
            std::cout << "Failed to open the file." << std::endl;
        }
    }

    ~CSVReader() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    std::vector<std::string> delimit(const char* delimiters) {
        std::vector<std::string> parsedData;
        std::string line;

        if (std::getline(m_file, line)) {
            std::istringstream ss(line);
            std::string token;

            while (std::getline(ss, token)) {
                size_t pos = 0;
                std::string delimiter;

                while (pos != std::string::npos) {
                    pos = token.find_first_of(delimiters);
                    std::string tokenPart = token.substr(0, pos);

                    if (!tokenPart.empty()) {
                        parsedData.push_back(tokenPart);
                    }

                    if (pos != std::string::npos) {
                        delimiter = token[pos];
                        token.erase(0, pos + 1);
                    }
                }

                if (!token.empty()) {
                    parsedData.push_back(token);
                }
            }
        }

        return parsedData;
    }

private:
    std::ifstream m_file;
};