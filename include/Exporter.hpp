#pragma once
#include <string>
#include <vector>
#include <variant>
#include "WorkoutRecord.hpp"
#include "Statistics.hpp"

namespace fitness {

struct CsvFormat  {};
struct TextFormat {};
using ExportFormat = std::variant<CsvFormat, TextFormat>;
class Exporter {
public:
    static void exportData(
        const std::vector<WorkoutRecord>& records,
        const FitnessStats& stats,
        const std::string& path,
        const ExportFormat& format
    );
    static void printToConsole(
        const std::vector<WorkoutRecord>& records,
        const FitnessStats& stats
    );
private:
    static void writeCsv (const std::vector<WorkoutRecord>& r, const FitnessStats& s, const std::string& path);
    static void writeText(const std::vector<WorkoutRecord>& r, const FitnessStats& s, const std::string& path);
};
}