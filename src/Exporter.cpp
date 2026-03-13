#include "Exporter.hpp"
#include "FitnessException.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace fitness {

void Exporter::exportData(
    const std::vector<WorkoutRecord>& records,
    const FitnessStats& stats,
    const std::string& path,
    const ExportFormat& format)
{
    std::visit([&](const auto& fmt) {
        using T = std::decay_t<decltype(fmt)>;
        if constexpr (std::is_same_v<T, CsvFormat>) {
            writeCsv(records, stats, path);
        } else if constexpr (std::is_same_v<T, TextFormat>) {
            writeText(records, stats, path);
        }
    }, format);
}

void Exporter::printToConsole(
    const std::vector<WorkoutRecord>& records,
    const FitnessStats& stats)
{
    std::cout << "\nWorkouts (" << records.size() << "):\n";
    std::cout << std::string(75, '-') << "\n";
    std::cout << std::left
              << std::setw(12) << "Date"
              << std::setw(10) << "Type"
              << std::setw(8)  << "Min"
              << std::setw(9)  << "Km"
              << std::setw(8)  << "Kcal"
              << std::setw(8)  << "BPM"
              << "Notes\n";
    std::cout << std::string(75, '-') << "\n";

    for (const auto& r : records) {
        std::cout << std::fixed << std::setprecision(1)
                  << std::left
                  << std::setw(12) << r.date
                  << std::setw(10) << workoutTypeToString(r.type)
                  << std::setw(8)  << r.duration
                  << std::setw(9)  << r.distance
                  << std::setw(8)  << r.calories
                  << std::setw(8)  << r.heart_rate
                  << r.notes << "\n";
    }
    std::cout << "\n" << Statistics::format(stats);
}

void Exporter::writeCsv(
    const std::vector<WorkoutRecord>& records,
    const FitnessStats& stats,
    const std::string& path)
{
    std::ofstream f(path);
    if (!f.is_open()) throw FileIOException(path);

    f << "date,type,duration_min,distance_km,calories,heart_rate_avg,notes\n";
    f << std::fixed << std::setprecision(1);
    for (const auto& r : records) {
        f << r.date                        << ","
          << workoutTypeToString(r.type)   << ","
          << r.duration                    << ","
          << r.distance                    << ","
          << r.calories                    << ","
          << r.heart_rate                  << ","
          << r.notes                       << "\n";
    }
    f << "\n# --- Summary ---\n";
    f << "# Total workouts: " << stats.total_workouts  << "\n";
    f << "# Total time:     " << stats.total_min       << " min\n";
    f << "# Total distance: " << stats.total_km        << " km\n";
    f << "# Total calories: " << stats.total_calories  << " kcal\n";
}

void Exporter::writeText(
    const std::vector<WorkoutRecord>& records,
    const FitnessStats& stats,
    const std::string& path)
{
    std::ofstream f(path);
    if (!f.is_open()) throw FileIOException(path);

    f << "FITNESS TRACKER REPORT\n";
    f << std::string(50, '=') << "\n\n";

    f << "WORKOUTS (" << records.size() << "):\n";
    f << std::string(50, '-') << "\n";
    f << std::fixed << std::setprecision(1);
    for (const auto& r : records) {
        f << r.date << "  "
          << std::left << std::setw(10) << workoutTypeToString(r.type)
          << std::right
          << std::setw(4) << r.duration << " min  "
          << std::setw(5) << r.distance << " km  "
          << std::setw(4) << r.calories << " kcal  "
          << std::setw(3) << r.heart_rate << " bpm";
        if (!r.notes.empty()) f << "  " << r.notes;
        f << "\n";
    }
    f << "\n" << Statistics::format(stats);
}

}
