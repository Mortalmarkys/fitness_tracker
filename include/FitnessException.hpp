#pragma once
#include <stdexcept>
#include <string>

namespace fitness {
    class FitnessException : public std::runtime_error {
    public:
        explicit FitnessException(const std::string& msg): std::runtime_error(msg) {}
    };

    class FileIOException : public FitnessException {
    public:
        explicit FileIOException(const std::string& path): FitnessException("Cannot open file: " + path) {}
    };

    class ParseException : public FitnessException {
    public:
        ParseException(const std::string& reason, int line)
            : FitnessException("Parse error at line " + std::to_string(line)
                               + ": " + reason) {}
    };

    class EmptyResultException : public FitnessException {
    public:
        EmptyResultException()
            : FitnessException("No workouts match the given filters") {}
    };
}
