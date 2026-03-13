#include <gtest/gtest.h>
#include <cmath>
#include "Statistics.hpp"
#include "FitnessException.hpp"

using namespace fitness;

static std::vector<WorkoutRecord> makeSample() {
    return {
        {"2024-01-10", WorkoutType::Running,  30, 5.0f, 300, 150, ""},
        {"2024-01-12", WorkoutType::Running,  60, 10.0f,500, 148, ""},
        {"2024-02-01", WorkoutType::Cycling,  90, 25.0f,600, 135, ""},
        {"2024-02-05", WorkoutType::Gym,      45, 0.0f, 350, 120, ""},
    };
}

TEST(StatisticsTest, ComputesTotals) {
    auto records = makeSample();
    auto stats = Statistics::compute(records);

    EXPECT_EQ(stats.total_workouts,  4);
    EXPECT_EQ(stats.total_min,       30 + 60 + 90 + 45); // 225
    EXPECT_FLOAT_EQ(stats.total_km,  5.0f + 10.0f + 25.0f + 0.0f); // 40.0
    EXPECT_EQ(stats.total_calories,  300 + 500 + 600 + 350); // 1750
}

TEST(StatisticsTest, ComputesAverages) {
    auto records = makeSample();
    auto stats = Statistics::compute(records);

    EXPECT_NEAR(stats.avg_duration,   (30.f+60+90+45)/4.f, 0.01f);
    EXPECT_NEAR(stats.avg_calories,   (300.f+500+600+350)/4.f, 0.01f);
    EXPECT_NEAR(stats.avg_heart_rate, (150.f+148+135+120)/4.f, 0.01f);
}

TEST(StatisticsTest, ComputesMaxValues) {
    auto records = makeSample();
    auto stats = Statistics::compute(records);

    EXPECT_EQ(stats.max_duration,    90);
    EXPECT_FLOAT_EQ(stats.max_distance, 25.0f);
    EXPECT_EQ(stats.max_calories,    600);
}

TEST(StatisticsTest, GroupsByType) {
    auto records = makeSample();
    auto stats = Statistics::compute(records);

    // Running: 2 тренировки
    ASSERT_NE(stats.by_type.find(WorkoutType::Running), stats.by_type.end());
    EXPECT_EQ(stats.by_type.at(WorkoutType::Running).count, 2);
    EXPECT_EQ(stats.by_type.at(WorkoutType::Running).total_min, 90);

    // Cycling: 1 тренировка
    ASSERT_NE(stats.by_type.find(WorkoutType::Cycling), stats.by_type.end());
    EXPECT_EQ(stats.by_type.at(WorkoutType::Cycling).count, 1);
}

TEST(StatisticsTest, TracksDates) {
    auto records = makeSample();
    auto stats = Statistics::compute(records);

    EXPECT_EQ(stats.date_first, "2024-01-10");
    EXPECT_EQ(stats.date_last,  "2024-02-05");
}

TEST(StatisticsTest, ThrowsOnEmpty) {
    std::vector<WorkoutRecord> empty;
    EXPECT_THROW(Statistics::compute(empty), EmptyResultException);
}

TEST(StatisticsTest, SpeedCalculation) {
    WorkoutRecord r;
    r.date = "2024-01-01";
    r.type = WorkoutType::Running;
    r.duration = 60;   // 1 час
    r.distance = 10.0f; // 10 км
    r.calories = 500;
    r.heart_rate = 150;

    // 10 км за 1 час = 10 км/ч
    EXPECT_FLOAT_EQ(r.speedKmh(), 10.0f);
}

TEST(StatisticsTest, CaloriesPerMin) {
    WorkoutRecord r;
    r.date = "2024-01-01";
    r.type = WorkoutType::Gym;
    r.duration = 50;
    r.distance = 0.0f;
    r.calories = 400;
    r.heart_rate = 120;

    // 400 / 50 = 8.0 ккал/мин
    EXPECT_FLOAT_EQ(r.caloriesPerMin(), 8.0f);
}

TEST(StatisticsTest, FormatContainsKeywords) {
    auto stats = Statistics::compute(makeSample());
    std::string text = Statistics::format(stats);

    EXPECT_NE(text.find("TOTALS"),    std::string::npos);
    EXPECT_NE(text.find("AVERAGES"),  std::string::npos);
    EXPECT_NE(text.find("BY TYPE"),   std::string::npos);
}
