#pragma once
#include <vector>
#include <string>
#include <memory>
#include "WorkoutRecord.hpp"

namespace fitness {

class Filter {
public:
    virtual ~Filter() = default;
    virtual std::vector<WorkoutRecord> apply(const std::vector<WorkoutRecord>& records) const = 0;
    virtual std::string describe() const = 0;
};

class TypeFilter : public Filter {
public:
    explicit TypeFilter(WorkoutType type);
    std::vector<WorkoutRecord> apply(const std::vector<WorkoutRecord>& r) const override;
    std::string describe() const override;
private:
    WorkoutType type_;
};

class DateRangeFilter : public Filter {
public:
    DateRangeFilter(std::string from, std::string to);
    std::vector<WorkoutRecord> apply(const std::vector<WorkoutRecord>& r) const override;
    std::string describe() const override;
private:
    std::string from_, to_;
};

class MinDurationFilter : public Filter {
public:
    explicit MinDurationFilter(int min_minutes);
    std::vector<WorkoutRecord> apply(const std::vector<WorkoutRecord>& r) const override;
    std::string describe() const override;
private:
    int min_minutes_;
};

class MinCaloriesFilter : public Filter {
public:
    explicit MinCaloriesFilter(int min_cal);
   std::vector<WorkoutRecord> apply(const std::vector<WorkoutRecord>& r) const override;
   std::string describe() const override;
private:
    int min_cal_;
};

class MinDistanceFilter : public Filter {
public:
    explicit MinDistanceFilter(float min_km);
    std::vector<WorkoutRecord> apply(const std::vector<WorkoutRecord>& r) const override;
    std::string describe() const override;
private:
    float min_km_;
};

class FilterPipeline {
public:
    void addFilter(std::unique_ptr<Filter> filter);
    std::vector<WorkoutRecord>
    apply(const std::vector<WorkoutRecord>& records) const;
    size_t size() const { return filters_.size(); }
    std::string describe() const;

private:
    std::vector<std::unique_ptr<Filter>> filters_;
};
}