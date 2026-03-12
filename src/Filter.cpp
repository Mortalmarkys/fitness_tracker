#include "Filter.hpp"
#include <sstream>

namespace fitness {

TypeFilter::TypeFilter(WorkoutType type) : type_(type) {}
std::vector<WorkoutRecord> TypeFilter::apply(const std::vector<WorkoutRecord>& records) const {
    std::vector<WorkoutRecord> result;
    for (const auto& r : records) {
        if (r.type == type_) result.push_back(r);
    }
    return result;
}

std::string TypeFilter::describe() const {
    return "type = " + workoutTypeToString(type_);
}

DateRangeFilter::DateRangeFilter(std::string from, std::string to): from_(std::move(from)), to_(std::move(to)) {}

std::vector<WorkoutRecord> DateRangeFilter::apply(const std::vector<WorkoutRecord>& records) const {
    std::vector<WorkoutRecord> result;
    for (const auto& r : records) {
        if (r.date >= from_ && r.date <= to_) result.push_back(r);
    }
    return result;
}

std::string DateRangeFilter::describe() const {
    return "date in [" + from_ + ", " + to_ + "]";
}

MinDurationFilter::MinDurationFilter(int min_minutes) : min_minutes_(min_minutes) {}

std::vector<WorkoutRecord> MinDurationFilter::apply(const std::vector<WorkoutRecord>& records) const {
    std::vector<WorkoutRecord> result;
    for (const auto& r : records) {
        if (r.duration >= min_minutes_) result.push_back(r);
    }
    return result;
}

std::string MinDurationFilter::describe() const {
    return "duration >= " + std::to_string(min_minutes_) + " min";
}

MinCaloriesFilter::MinCaloriesFilter(int min_cal) : min_cal_(min_cal) {}

std::vector<WorkoutRecord> MinCaloriesFilter::apply(const std::vector<WorkoutRecord>& records) const {
    std::vector<WorkoutRecord> result;
    for (const auto& r : records) {
        if (r.calories >= min_cal_) result.push_back(r);
    }
    return result;
}

std::string MinCaloriesFilter::describe() const {
    return "calories >= " + std::to_string(min_cal_);
}

MinDistanceFilter::MinDistanceFilter(float min_km) : min_km_(min_km) {}

std::vector<WorkoutRecord> MinDistanceFilter::apply(const std::vector<WorkoutRecord>& records) const {
    std::vector<WorkoutRecord> result;
    for (const auto& r : records) {
        if (r.distance >= min_km_) result.push_back(r);
    }
    return result;
}

std::string MinDistanceFilter::describe() const {
    return "distance >= " + std::to_string(min_km_) + " km";
}

void FilterPipeline::addFilter(std::unique_ptr<Filter> filter) {
    filters_.push_back(std::move(filter));
}

std::vector<WorkoutRecord> FilterPipeline::apply(const std::vector<WorkoutRecord>& records) const {
    std::vector<WorkoutRecord> current = records;
    for (const auto& f : filters_) {
        current = f->apply(current);
        if (current.empty()) break;
    }
    return current;
}

std::string FilterPipeline::describe() const {
    if (filters_.empty()) return "(no filters)";
    std::ostringstream oss;
    for (size_t i = 0; i < filters_.size(); ++i) {
        if (i > 0) oss << " AND ";
        oss << filters_[i]->describe();
    }
    return oss.str();
}
}
