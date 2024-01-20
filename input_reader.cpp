#include <algorithm>
#include "input_reader.h"
#include <iostream>
#include <string>

using namespace std;

namespace transport::input {
/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
    std::string_view Trim(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

vector<catalogue::Distance> ParseDistance(std::string_view str) {
    auto comma = str.find(',');
    comma = str.find(',', comma + 1);

    if (comma == str.npos) {
        return {};
    }

    vector<catalogue::Distance> distances;

    str = Trim(str.substr(comma + 1));
    while (str.length() > 0) {
        comma = str.find(',');
        string_view work_str = str.substr(0, comma);
        auto meter = work_str.find('m');
        int dist = stoi(string(work_str.substr(0, meter + 1)));
        work_str = Trim(work_str.substr(meter + 1));
        work_str = Trim(work_str.substr((work_str.find_first_not_of("to"))));
        distances.push_back({pair<std::string, std::string>{"1", work_str}, dist});

        str = comma != str.npos ? Trim(str.substr(comma + 1)) : "";
    }

    return distances;
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] catalogue::TransportCatalogue& catalogue) const {
    for (auto& command : commands_) {
        if (command.command == "Stop") {
            catalogue::Stop stop = {command.id, ParseCoordinates(command.description)};
            catalogue.AddStop(stop);
        }
    }

    for (auto& command : commands_) {
        if (command.command == "Stop") {
            vector<catalogue::Distance> distance = ParseDistance(command.description);
            for (auto& distance_elem : distance) {
                distance_elem.stop_pair.pair_stop.first = command.id;
                catalogue.AddDistance(distance_elem);
            }
        }
    }

    for (auto& command : commands_) {
        if (command.command == "Bus") {
            vector<string_view> stops = ParseRoute(command.description);
            catalogue::Bus bus = {command.id, vector<transport::catalogue::Stop*>()};
            for (auto& stop : stops) {
                bus.stop_names.push_back(catalogue.FindStop(stop));
            }
            catalogue.AddBus(bus);
        }
    }
}

void InputReader::ReadInput(std::istream& in) {
    int base_request_count;
    in >> base_request_count >> ws;
    for (int i = 0; i < base_request_count; ++i) {
        string line;
        getline(in, line);
        ParseLine(line);
    }
}
}