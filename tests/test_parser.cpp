#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "CsvParser.hpp"
#include "FitnessException.hpp"

using namespace fitness;

static std::string writeTempFile(const std::string& content) {
    std::string path = (std::filesystem::temp_directory_path()
                        / "fitness_test.csv").string();
    std::ofstream f(path);
    f << content;
    return path;
}

TEST(CsvParserTest, LoadsValidFile) {
    std::string path = writeTempFile(
        "date,type,duration_min,distance_km,calories,heart_rate_avg,notes\n"
        "2024-01-01,Running,35,5.2,320,145,Morning\n"
        "2024-01-02,Gym,50,0.0,380,120,Chest day\n"
    );
    CsvParser parser;
    auto result = parser.load(path);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 2u);
    EXPECT_EQ((*result)[0].type, WorkoutType::Running);
    EXPECT_EQ((*result)[1].type, WorkoutType::Gym);
    EXPECT_EQ((*result)[0].duration, 35);
    EXPECT_FLOAT_EQ((*result)[0].distance, 5.2f);
}

TEST(CsvParserTest, ThrowsOnMissingFile) {
    CsvParser parser;
    EXPECT_THROW(parser.load("no_such_file.csv"), FileIOException);
}

TEST(CsvParserTest, EmptyFileReturnsEmptyVector) {
    std::string path = writeTempFile(
        "date,type,duration_min,distance_km,calories,heart_rate_avg,notes\n"
    );
    CsvParser parser;
    auto result = parser.load(path);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->empty());
}

TEST(CsvParserTest, SkipsBadLines) {
    std::string path = writeTempFile(
        "date,type,duration_min,distance_km,calories,heart_rate_avg,notes\n"
        "2024-01-01,Running,35,5.2,320,145,OK\n"
        "2024-01-02,UNKNOWN_TYPE,50,0.0,380,120,bad\n"  // плохой тип
        "2024-01-03,Gym,55,0.0,400,121,OK\n"
    );
    CsvParser parser;
    int skipped = 0;
    auto result = parser.load(path, &skipped);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_EQ(skipped, 1);
}

TEST(CsvParserTest, SkipsOutOfRangeValues) {
    std::string path = writeTempFile(
        "date,type,duration_min,distance_km,calories,heart_rate_avg,notes\n"
        "2024-01-01,Running,9999,5.2,320,145,too long\n"  // 9999 мин > MAX
        "2024-01-02,Running,35,5.2,320,145,OK\n"
    );
    CsvParser parser;
    auto result = parser.load(path);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 1u);
}

TEST(CsvParserTest, WorkoutTypeConversion) {
    EXPECT_EQ(workoutTypeFromString("Running"),  WorkoutType::Running);
    EXPECT_EQ(workoutTypeFromString("running"),  WorkoutType::Running); // регистр
    EXPECT_EQ(workoutTypeFromString("CYCLING"),  WorkoutType::Cycling);
    EXPECT_EQ(workoutTypeFromString("Swimming"), WorkoutType::Swimming);
    EXPECT_EQ(workoutTypeFromString("Gym"),      WorkoutType::Gym);
    EXPECT_EQ(workoutTypeFromString("Yoga"),     WorkoutType::Unknown);

    EXPECT_EQ(workoutTypeToString(WorkoutType::Running),  "Running");
    EXPECT_EQ(workoutTypeToString(WorkoutType::Cycling),  "Cycling");
    EXPECT_EQ(workoutTypeToString(WorkoutType::Swimming), "Swimming");
    EXPECT_EQ(workoutTypeToString(WorkoutType::Gym),      "Gym");
}
