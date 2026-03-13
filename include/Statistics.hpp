#pragma once
#include <vector>
#include <string>
#include <map>
#include "WorkoutRecord.hpp"

namespace fitness {

struct TypeStats {
    int count = 0;
    int total_min = 0;
    float total_km = 0.0f;
    int total_cal = 0;
    float avg_heart  = 0.0f;
    float avg_speed = 0.0f;
    float best_speed = 0.0f;
};

struct FitnessStats {

    int total_workouts = 0;
    int total_min = 0;
    float total_km = 0.0f;
    int total_calories = 0;
    float avg_duration = 0.0f;
    float avg_calories = 0.0f;
    float avg_heart_rate = 0.0f;
    int max_duration = 0;
    float max_distance = 0.0f;
    int max_calories = 0;
    float max_speed = 0.0f;

    std::map<WorkoutType, TypeStats> by_type;

    std::string date_first;
    std::string date_last;

    int longest_streak = 0;
};

class Statistics {
public:

    static FitnessStats compute(const std::vector<WorkoutRecord>& records);
    static std::string format(const FitnessStats& stats);
    static std::string formatTypeStats(WorkoutType type, const TypeStats& ts);

private:
    static float calcMean(const std::vector<float>& v);
    static int   calcLongestStreak(const std::vector<WorkoutRecord>& records);
};
}