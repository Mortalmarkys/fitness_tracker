#include "CsvParser.hpp"
#include "WorkoutRecord.hpp"
#include "FitnessException.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace fitness {

WorkoutType workoutTypeFromString(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c){ return std::tolower(c); });
    if (lower == "running") return WorkoutType::Running;
    if (lower == "cycling") return WorkoutType::Cycling;
    if (lower == "swimming") return WorkoutType::Swimming;
    if (lower == "gym")     return WorkoutType::Gym;
    return WorkoutType::Unknown;
}

std::string workoutTypeToString(WorkoutType t) {
    switch (t) {
        case WorkoutType::Running: return "Running";
        case WorkoutType::Cycling: return "Cycling";
        case WorkoutType::Swimming: return "Swimming";
        case WorkoutType::Gym: return "Gym";
        default: return "Unknown";
    }
}

static std::string trim(const std::string& s) {
    std::string::const_iterator start = std::find_if(s.begin(), s.end(),
        [](unsigned char c){ return !std::isspace(c); });
    std::string::const_iterator end = std::find_if(s.rbegin(), s.rend(),
        [](unsigned char c){ return !std::isspace(c); }).base();
    if (start < end) {
        return std::string(start, end);
    } else {
        return std::string{};
    }
}

std::vector<std::string> CsvParser::split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::istringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        result.push_back(trim(token));
    }
    return result;
}

std::optional<WorkoutRecord> CsvParser::parseLine(const std::string& line, int line_number) const{
    if (line.empty() || line[0] == '#') return std::nullopt;
    std::vector<std::string> f = split(line, ',');
    if (f.size() < 6) return std::nullopt;
    WorkoutRecord rec;
    rec.date = f[0];
    rec.type = workoutTypeFromString(f[1]);
    if (rec.type == WorkoutType::Unknown) return std::nullopt;

    try {
        rec.duration   = std::stoi(f[2]);
        rec.distance   = std::stof(f[3]);
        rec.calories   = std::stoi(f[4]);
        rec.heart_rate = std::stoi(f[5]);
    } catch (const std::exception&) {
        return std::nullopt;
    }
    if (f.size() >= 7) rec.notes = f[6];
    if (!rec.isValid()) return std::nullopt;
    return rec;
}


std::optional<std::vector<WorkoutRecord>> CsvParser::load(const std::string& path, int* skipped_lines) const
{
    std::ifstream file(path);
    if (!file.is_open()) throw FileIOException(path);
    std::vector<WorkoutRecord> records;
    std::string line;
    int line_number = 0;
    int skipped = 0;
    bool header_skipped = false;
    while (std::getline(file, line)) {
        ++line_number;
        if (!header_skipped && !line.empty() && line[0] != '#') {
            header_skipped = true;
            continue;
        }

        std::optional<WorkoutRecord> rec = parseLine(line, line_number);
        if (rec.has_value()) {
            records.push_back(std::move(*rec));
        } else if (!line.empty() && line[0] != '#') {
            ++skipped;
        }
    }

    if (skipped_lines) *skipped_lines = skipped;
    return records;
}

}
