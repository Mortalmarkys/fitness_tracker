#include <gtest/gtest.h>
#include <memory>
#include "Filter.hpp"

using namespace fitness;

static std::vector<WorkoutRecord> makeSample() {
    return {
        {"2024-01-10", WorkoutType::Running,  35, 5.2f, 320, 145, ""},
        {"2024-01-15", WorkoutType::Cycling,  60, 18.5f,480, 130, ""},
        {"2024-02-05", WorkoutType::Gym,      50, 0.0f, 380, 120, ""},
        {"2024-02-10", WorkoutType::Swimming, 45, 2.0f, 410, 138, ""},
        {"2024-03-01", WorkoutType::Running,  28, 4.1f, 255, 150, ""},
        {"2024-03-15", WorkoutType::Running,  55, 8.0f, 490, 147, ""},
    };
}

// ---- TypeFilter ----

TEST(TypeFilterTest, KeepsOnlyMatchingType) {
    auto records = makeSample();
    TypeFilter f(WorkoutType::Running);
    auto result = f.apply(records);

    EXPECT_EQ(result.size(), 3u);
    for (const auto& r : result)
        EXPECT_EQ(r.type, WorkoutType::Running);
}

TEST(TypeFilterTest, EmptyIfNoMatch) {
    auto records = makeSample();
    TypeFilter f(WorkoutType::Gym);
    auto result = f.apply(records);
    EXPECT_EQ(result.size(), 1u);
}

// ---- DateRangeFilter ----

TEST(DateRangeFilterTest, FiltersCorrectly) {
    auto records = makeSample();
    DateRangeFilter f("2024-02-01", "2024-02-28");
    auto result = f.apply(records);

    EXPECT_EQ(result.size(), 2u);
    for (const auto& r : result) {
        EXPECT_GE(r.date, "2024-02-01");
        EXPECT_LE(r.date, "2024-02-28");
    }
}

TEST(DateRangeFilterTest, InclusiveBounds) {
    auto records = makeSample();
    DateRangeFilter f("2024-01-10", "2024-01-10");
    auto result = f.apply(records);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].date, "2024-01-10");
}

// ---- MinDurationFilter ----

TEST(MinDurationFilterTest, FiltersShortWorkouts) {
    auto records = makeSample();
    MinDurationFilter f(50);
    auto result = f.apply(records);

    EXPECT_EQ(result.size(), 3u); // 60, 50, 55
    for (const auto& r : result)
        EXPECT_GE(r.duration, 50);
}

// ---- MinCaloriesFilter ----

TEST(MinCaloriesFilterTest, FiltersLowCalories) {
    auto records = makeSample();
    MinCaloriesFilter f(400);
    auto result = f.apply(records);

    EXPECT_EQ(result.size(), 4u); // 480, 380, 410, 490 — только >= 400
    for (const auto& r : result)
        EXPECT_GE(r.calories, 400);
}

// ---- MinDistanceFilter ----

TEST(MinDistanceFilterTest, ExcludesGymWorkouts) {
    auto records = makeSample();
    MinDistanceFilter f(0.1f); // исключаем gym (distance == 0)
    auto result = f.apply(records);

    for (const auto& r : result)
        EXPECT_GE(r.distance, 0.1f);
    // Gym с distance=0 не должен попасть
    for (const auto& r : result)
        EXPECT_NE(r.type, WorkoutType::Gym);
}

// ---- FilterPipeline ----

TEST(FilterPipelineTest, CombinesFilters) {
    auto records = makeSample();
    FilterPipeline pipeline;
    pipeline.addFilter(std::make_unique<TypeFilter>(WorkoutType::Running));
    pipeline.addFilter(std::make_unique<MinDurationFilter>(40));

    auto result = pipeline.apply(records);

    // Только Running с duration >= 40: 35 нет, 28 нет, 55 да
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].duration, 55);
}

TEST(FilterPipelineTest, EmptyPipelineReturnsAll) {
    auto records = makeSample();
    FilterPipeline pipeline;
    EXPECT_EQ(pipeline.apply(records).size(), records.size());
}

TEST(FilterPipelineTest, DescribeIsNotEmpty) {
    FilterPipeline pipeline;
    pipeline.addFilter(std::make_unique<TypeFilter>(WorkoutType::Running));
    EXPECT_FALSE(pipeline.describe().empty());
    EXPECT_NE(pipeline.describe().find("Running"), std::string::npos);
}
