#include "Statistics.hpp"
#include "FitnessException.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <set>
#include <sstream>
#include <iomanip>

namespace fitness {

float Statistics::calcMean(const std::vector<float>& v) {
    if (v.empty()) return 0.0f;
    return std::accumulate(v.begin(), v.end(), 0.0f) / static_cast<float>(v.size());
}

int Statistics::calcLongestStreak(const std::vector<WorkoutRecord>& records) {
    if (records.empty()) return 0;
    std::set<std::string> dates_set;
    for (const auto& r : records) dates_set.insert(r.date);
    std::vector<std::string> dates(dates_set.begin(), dates_set.end());
    int longest = 1, current = 1;
    for (size_t i = 1; i < dates.size(); ++i) {
        int y0, m0, d0, y1, m1, d1;
        if (std::sscanf(dates[i-1].c_str(), "%d-%d-%d", &y0, &m0, &d0) == 3 &&
            std::sscanf(dates[i  ].c_str(), "%d-%d-%d", &y1, &m1, &d1) == 3)
        {
            int day0 = y0 * 365 + m0 * 30 + d0;
            int day1 = y1 * 365 + m1 * 30 + d1;

            if (day1 - day0 == 1) {
                ++current;
                longest = std::max(longest, current);
            } else {
                current = 1;
            }
        }
    }

    return longest;
}

FitnessStats Statistics::compute(const std::vector<WorkoutRecord>& records) {
    if (records.empty()) throw EmptyResultException();

    FitnessStats stats;
    stats.total_workouts = static_cast<int>(records.size());

    std::vector<float> durations, calories, heart_rates;

    for (const auto& r : records) {
        stats.total_min += r.duration;
        stats.total_km += r.distance;
        stats.total_calories += r.calories;

        stats.max_duration = std::max(stats.max_duration, r.duration);
        stats.max_distance = std::max(stats.max_distance, r.distance);
        stats.max_calories = std::max(stats.max_calories, r.calories);

        float spd = r.speedKmh();
        stats.max_speed = std::max(stats.max_speed, spd);

        durations.push_back(static_cast<float>(r.duration));
        calories.push_back(static_cast<float>(r.calories));
        heart_rates.push_back(static_cast<float>(r.heart_rate));

        auto& ts = stats.by_type[r.type];
        ts.count++;
        ts.total_min += r.duration;
        ts.total_km  += r.distance;
        ts.total_cal += r.calories;
        ts.avg_heart = (ts.avg_heart * (ts.count - 1) + r.heart_rate) / ts.count;
        if (spd > ts.best_speed) ts.best_speed = spd;
    }

    stats.avg_duration = calcMean(durations);
    stats.avg_calories = calcMean(calories);
    stats.avg_heart_rate = calcMean(heart_rates);

    for (auto& [type, ts] : stats.by_type) {
        if (ts.total_min > 0 && ts.total_km > 0) {
            ts.avg_speed = ts.total_km / (static_cast<float>(ts.total_min) / 60.0f);
        }
    }

    stats.date_first = records.front().date;
    stats.date_last  = records.back().date;
    for (const auto& r : records) {
        if (r.date < stats.date_first) stats.date_first = r.date;
        if (r.date > stats.date_last)  stats.date_last  = r.date;
    }

    stats.longest_streak = calcLongestStreak(records);

    return stats;
}

std::string Statistics::formatTypeStats(WorkoutType type, const TypeStats& ts) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    oss << "  " << std::left << std::setw(10) << workoutTypeToString(type)
        << "  sessions: " << std::right << std::setw(3) << ts.count
        << "   time: " << std::setw(5) << ts.total_min << " min"
        << "   cal: "  << std::setw(5) << ts.total_cal;
    if (ts.total_km > 0) {
        oss << "   dist: " << std::setw(6) << ts.total_km << " km"
            << "   avg speed: " << std::setw(4) << ts.avg_speed << " km/h";
    }
    oss << "\n";
    return oss.str();
}

std::string Statistics::format(const FitnessStats& stats) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);

    oss << "==========================================\n";
    oss << "   FITNESS STATISTICS\n";
    oss << "==========================================\n";
    oss << "  Workouts     : " << stats.total_workouts  << "\n";
    oss << "  Period       : " << stats.date_first << " to " << stats.date_last << "\n";
    oss << "  Longest streak: " << stats.longest_streak << " days\n";
    oss << "------------------------------------------\n";
    oss << "  TOTALS\n";
    oss << "    Time       : " << stats.total_min      << " min ("
                               << stats.total_min / 60 << "h "
                               << stats.total_min % 60 << "m)\n";
    oss << "    Distance   : " << stats.total_km       << " km\n";
    oss << "    Calories   : " << stats.total_calories << " kcal\n";
    oss << "------------------------------------------\n";
    oss << "  AVERAGES PER SESSION\n";
    oss << "    Duration   : " << stats.avg_duration   << " min\n";
    oss << "    Calories   : " << stats.avg_calories   << " kcal\n";
    oss << "    Heart Rate : " << stats.avg_heart_rate << " bpm\n";
    oss << "------------------------------------------\n";
    oss << "  PERSONAL BESTS\n";
    oss << "    Longest    : " << stats.max_duration   << " min\n";
    oss << "    Farthest   : " << stats.max_distance   << " km\n";
    oss << "    Most cal   : " << stats.max_calories   << " kcal\n";
    oss << "    Top speed  : " << stats.max_speed      << " km/h\n";
    oss << "------------------------------------------\n";
    oss << "  BY TYPE\n";
    for (const auto& [type, ts] : stats.by_type) {
        oss << formatTypeStats(type, ts);
    }
    oss << "==========================================\n";

    return oss.str();
}
}
