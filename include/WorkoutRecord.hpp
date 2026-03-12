#pragma once
#include <string>

namespace fitness {
    
enum class WorkoutType {
    Running,
    Cycling,
    Swimming,
    Gym,
    Unknown
};

constexpr int Min_Duration_Min = 1;
constexpr int Max_Duration_Min = 600;
constexpr float Min_Distance_KM = 0.0f;
constexpr float Max_Distance_KM = 300.0f;
constexpr int Min_calories = 1;
constexpr int Max_calories = 5000;
constexpr int Min_Heart_Rate = 40;
constexpr int Max_Heart_Rate = 220;

WorkoutType     workoutTypeFromString(const std::string& s);
std::string     workoutTypeToString(WorkoutType t);

struct WorkoutRecord {
    std::string date;
    WorkoutType type = WorkoutType::Unknown;
    int duration = 0;
    float distance = 0.0f;
    int calories = 0;
    int heart_rate = 0;
    std::string notes;

    bool isValid() const {
        return !date.empty()
            && type != WorkoutType::Unknown
            && duration >= Min_Duration_Min && duration <= Max_Duration_Min
            && distance >= Min_Distance_KM && distance <= Max_Distance_KM
            && calories >= Min_Calories && calories <= Max_Calories
            && heart_rate >= Min_Heart_Rate && heart_rate <= Max_Heart_Rate;
    }

    float speedKmh() const {
        if (distance <= 0.0f || duration <= 0) return 0.0f;
        return distance / (static_cast<float>(duration) / 60.0f);
    }

    float caloriesPerMin() const {
        if (duration <= 0) return 0.0f;
        return static_cast<float>(calories) / static_cast<float>(duration);
    }
};
}