#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

class CSV {
    static constexpr size_t DEFAULT_MAX_LINE_LENGTH = 256;

public:

    CSV(const std::string& filename) : m_file_name(filename), m_file(filename), 
        m_max_line_length(DEFAULT_MAX_LINE_LENGTH), m_line_count(0) {
        if(!m_file.is_open())
            throw std::ios_base::failure("File not found: " + m_file_name);

        std::string line;
        while (std::getline(m_file, line))
            ++m_line_count;

        // Cycle the file
        m_file.close();
        m_file.open(m_file_name);
    }


    void open(const std::string& file_name) { m_file.open(file_name); m_file_name = file_name; }
    void close() { m_file.close(); }

    void setMaxLineLength(const size_t _length) { m_max_line_length = _length; }

    auto getLineCount() { return m_line_count; }
    auto getMaxLineLength() { return m_max_line_length; }
    std::ifstream& getFileStream() { return m_file; }

    std::vector<char> getNextLine() {
        std::string line;
        if (std::getline(m_file, line)) {
            return std::vector<char>(line.begin(), line.end());
        }
    }

    auto getNextLineUnparsed() {
        std::string line;
        if (std::getline(m_file, line)) {
            return line;
        }
    }

    std::vector<std::string> readLineDelimited(const char* delimiters, size_t numFields) {
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(m_file, field)) {
            size_t start = 0;
            size_t end = 0;
            size_t fieldCount = 0;

            while (fieldCount < numFields - 1) {
                for (size_t i = 0; i < std::strlen(delimiters); i++) {
                    end = field.find_first_of(delimiters[i], start);
                    if (end != std::string::npos) break;
                }

                if (end == std::string::npos) break;

                fields.push_back(field.substr(start, end - start));
                start = end + 1;
                fieldCount++;
            }

            // Handle the last field separately to avoid adding an extra delimiter
            fields.push_back(field.substr(start));

            if (fields.size() >= numFields) break;
        }

        return fields;
    }

private:
    std::string m_file_name;
    std::ifstream m_file;
    size_t m_max_line_length;
    size_t m_line_count;
};