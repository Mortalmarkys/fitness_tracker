#include <iostream>
#include <string>
#include <memory>
#include <optional>
#include <limits>
#include "WorkoutRecord.hpp"
#include "FitnessException.hpp"
#include "CsvParser.hpp"
#include "Filter.hpp"
#include "Statistics.hpp"
#include "Exporter.hpp"

using namespace fitness;

struct Args {
    std::string                input_file;
    std::optional<std::string> type;
    std::optional<std::string> date_from;
    std::optional<std::string> date_to;
    std::optional<int>         min_duration;
    std::optional<int>         min_calories;
    std::optional<float>       min_distance;
    std::optional<std::string> output_file;
    bool                       output_csv = false;
};

static void printLine(char c = '-', int n = 50) {
    std::cout << std::string(n, c) << "\n";
}

static std::string askString(const std::string& prompt) {
    std::cout << prompt;
    std::string val;
    std::getline(std::cin, val);
    return val;
}

static std::optional<int> askOptionalInt(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return std::nullopt;
    try { return std::stoi(line); }
    catch (...) { return std::nullopt; }
}

static std::optional<float> askOptionalFloat(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return std::nullopt;
    try { return std::stof(line); }
    catch (...) { return std::nullopt; }
}

static void showResults(const std::vector<WorkoutRecord>& all,
                        const FilterPipeline& pipeline)
{
    auto filtered = pipeline.apply(all);

    if (filtered.empty()) {
        std::cout << "\n  No workouts match the selected filters.\n";
        return;
    }

    try {
        auto stats = Statistics::compute(filtered);
        Exporter::printToConsole(filtered, stats);
    } catch (const EmptyResultException& e) {
        std::cout << "  " << e.what() << "\n";
    }
}

static void runInteractive(std::vector<WorkoutRecord>& all) {
    std::cout << "\n";
    printLine('=', 50);
    std::cout << "   FITNESS TRACKER\n";
    printLine('=', 50);
    std::cout << "  Workouts loaded: " << all.size() << "\n\n";

    while (true) {
        std::cout << "  What would you like to do?\n\n";
        std::cout << "    1  show all workouts\n";
        std::cout << "    2  filter by type\n";
        std::cout << "    3  filter by date range\n";
        std::cout << "    4  filter by calories and duration\n";
        std::cout << "    5  filter by distance\n";
        std::cout << "    6  full statistics\n";
        std::cout << "    7  save to file\n";
        std::cout << "    0  exit\n\n";
        std::cout << "  > ";

        std::string choice;
        std::getline(std::cin, choice);
        std::cout << "\n";

        if (choice == "0") {
            std::cout << "  Goodbye!\n";
            break;
        }

        else if (choice == "1") {
            FilterPipeline p;
            showResults(all, p);
        }

        else if (choice == "2") {
            std::cout << "  Available types: Running, Cycling, Swimming, Gym\n";
            std::string t = askString("  Enter type: ");
            WorkoutType wt = workoutTypeFromString(t);
            if (wt == WorkoutType::Unknown) {
                std::cout << "  Unknown type: \"" << t << "\"\n\n";
                continue;
            }
            FilterPipeline p;
            p.addFilter(std::make_unique<TypeFilter>(wt));
            showResults(all, p);
        }

        else if (choice == "3") {
            std::string from = askString("  Date from  (YYYY-MM-DD, Enter = any): ");
            std::string to   = askString("  Date to    (YYYY-MM-DD, Enter = any): ");

            FilterPipeline p;
            if (!from.empty() && !to.empty()) {
                p.addFilter(std::make_unique<DateRangeFilter>(from, to));
            } else if (!from.empty()) {
                p.addFilter(std::make_unique<DateRangeFilter>(from, "9999-12-31"));
            } else if (!to.empty()) {
                p.addFilter(std::make_unique<DateRangeFilter>("0000-01-01", to));
            }
            showResults(all, p);
        }

        else if (choice == "4") {
            auto min_cal = askOptionalInt(
                "  Min calories  (Enter = any): ");
            auto min_dur = askOptionalInt(
                "  Min minutes   (Enter = any): ");

            FilterPipeline p;
            if (min_cal.has_value())
                p.addFilter(std::make_unique<MinCaloriesFilter>(*min_cal));
            if (min_dur.has_value())
                p.addFilter(std::make_unique<MinDurationFilter>(*min_dur));
            showResults(all, p);
        }

        else if (choice == "5") {
            auto min_km = askOptionalFloat(
                "  Min distance (km, Enter = any): ");

            FilterPipeline p;
            if (min_km.has_value())
                p.addFilter(std::make_unique<MinDistanceFilter>(*min_km));
            showResults(all, p);
        }

        else if (choice == "6") {
            try {
                auto stats = Statistics::compute(all);
                std::cout << Statistics::format(stats);
            } catch (const EmptyResultException& e) {
                std::cout << "  " << e.what() << "\n";
            }
        }

        else if (choice == "7") {
            std::string path = askString("  File name (e.g. result.txt): ");
            if (path.empty()) {
                std::cout << "  File name cannot be empty.\n\n";
                continue;
            }
            std::cout << "  Format: 1 — text,  2 — CSV\n  > ";
            std::string fmt_choice;
            std::getline(std::cin, fmt_choice);

            ExportFormat fmt = (fmt_choice == "2")
                ? ExportFormat{CsvFormat{}}
                : ExportFormat{TextFormat{}};

            try {
                auto stats = Statistics::compute(all);
                Exporter::exportData(all, stats, path, fmt);
                std::cout << "  Saved: " << path << "\n";
            } catch (const std::exception& e) {
                std::cout << "  Error: " << e.what() << "\n";
            }
        }

        else {
            std::cout << "  Unknown command. Enter a number from 0 to 7.\n";
        }

        std::cout << "\n";
        printLine('=', 50);
        std::cout << "\n";
    }
}

void printHelp(const char* name) {
    std::cout << "Usage:\n";
    std::cout << "  " << name << " data/workouts.csv           — interactive menu\n";
    std::cout << "  " << name << " data/workouts.csv [options] — print results directly\n\n";
    std::cout << "Options:\n";
    std::cout << "  --type <t>           Running|Cycling|Swimming|Gym\n";
    std::cout << "  --from <YYYY-MM-DD>  date from\n";
    std::cout << "  --to   <YYYY-MM-DD>  date to\n";
    std::cout << "  --min-duration <n>   duration >= n min\n";
    std::cout << "  --min-calories <n>   calories >= n\n";
    std::cout << "  --min-distance <n>   distance >= n km\n";
    std::cout << "  --out <file>         save output to file\n";
    std::cout << "  --csv                CSV format (default: text)\n";
}

std::optional<Args> parseArgs(int argc, char* argv[]) {
    if (argc < 2) return std::nullopt;
    std::string first = argv[1];
    if (first == "--help" || first == "-h") {
        printHelp(argv[0]);
        std::exit(0);
    }
    Args args;
    args.input_file = first;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        try {
            if      (a == "--type"         && i+1 < argc) args.type         = argv[++i];
            else if (a == "--from"         && i+1 < argc) args.date_from    = argv[++i];
            else if (a == "--to"           && i+1 < argc) args.date_to      = argv[++i];
            else if (a == "--min-duration" && i+1 < argc) args.min_duration = std::stoi(argv[++i]);
            else if (a == "--min-calories" && i+1 < argc) args.min_calories = std::stoi(argv[++i]);
            else if (a == "--min-distance" && i+1 < argc) args.min_distance = std::stof(argv[++i]);
            else if (a == "--out"          && i+1 < argc) args.output_file  = argv[++i];
            else if (a == "--csv")                        args.output_csv   = true;
            else { std::cerr << "Unknown flag: " << a << "\n"; return std::nullopt; }
        } catch (...) {
            std::cerr << "Invalid value for " << a << "\n";
            return std::nullopt;
        }
    }
    return args;
}

int main(int argc, char* argv[]) {

    std::string input_file;
    bool interactive_mode = false;

    if (argc < 2) {
        std::cout << "Path to workouts file\n";
        std::cout << "(Enter = data/workouts.csv): ";
        std::getline(std::cin, input_file);
        if (input_file.empty()) input_file = "data/workouts.csv";
        interactive_mode = true;

    } else if (argc == 2) {
        input_file = argv[1];
        interactive_mode = true;

    } else {
        input_file = argv[1];
        interactive_mode = false;
    }

    try {
        CsvParser parser;
        int skipped = 0;
        auto records_opt = parser.load(input_file, &skipped);

        if (!records_opt.has_value() || records_opt->empty()) {
            std::cerr << "No data in file: " << input_file << "\n";
            return 1;
        }

        auto records = std::move(*records_opt);

        if (skipped > 0)
            std::cout << "Skipped invalid lines: " << skipped << "\n";

        if (interactive_mode) {
            runInteractive(records);
            return 0;
        }

        auto args_opt = parseArgs(argc, argv);
        if (!args_opt.has_value()) { printHelp(argv[0]); return 1; }
        const Args& args = *args_opt;

        FilterPipeline pipeline;
        if (args.type.has_value()) {
            WorkoutType wt = workoutTypeFromString(*args.type);
            if (wt == WorkoutType::Unknown) {
                std::cerr << "Unknown type: " << *args.type << "\n"; return 1;
            }
            pipeline.addFilter(std::make_unique<TypeFilter>(wt));
        }
        if (args.date_from.has_value() && args.date_to.has_value())
            pipeline.addFilter(std::make_unique<DateRangeFilter>(*args.date_from, *args.date_to));
        if (args.min_duration.has_value())
            pipeline.addFilter(std::make_unique<MinDurationFilter>(*args.min_duration));
        if (args.min_calories.has_value())
            pipeline.addFilter(std::make_unique<MinCaloriesFilter>(*args.min_calories));
        if (args.min_distance.has_value())
            pipeline.addFilter(std::make_unique<MinDistanceFilter>(*args.min_distance));

        auto filtered = pipeline.apply(records);
        auto stats    = Statistics::compute(filtered);
        Exporter::printToConsole(filtered, stats);

        if (args.output_file.has_value()) {
            ExportFormat fmt = args.output_csv
                ? ExportFormat{CsvFormat{}}
                : ExportFormat{TextFormat{}};
            Exporter::exportData(filtered, stats, *args.output_file, fmt);
            std::cout << "Saved: " << *args.output_file << "\n";
        }

    } catch (const FileIOException& e) {
        std::cerr << "File not found: " << e.what() << "\n"; return 1;
    } catch (const EmptyResultException& e) {
        std::cerr << "No results: "     << e.what() << "\n"; return 1;
    } catch (const FitnessException& e) {
        std::cerr << "Error: "          << e.what() << "\n"; return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: "<< e.what() << "\n"; return 1;
    }

    return 0;
}
