#pragma once
#include <vector>
#include <string>
#include <optional>
#include "WorkoutRecord.hpp"

namespace fitness {

class CsvParser {
public:
    std::optional<std::vector<WorkoutRecord>> load(const std::string& path, int* skipped_lines = nullptr) const;
private:
    static std::vector<std::string> split(const std::string& str, char delim);
    std::optional<WorkoutRecord> parseLine(const std::string& line, int line_number) const;
};

}
