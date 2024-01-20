#pragma once
#include <deque>
#include "geo.h"
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace transport::catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coord;
};

struct PairStop {
    std::pair<std::string, std::string> pair_stop;

    bool operator==(const PairStop& other) const {
        return (this->pair_stop.first == other.pair_stop.first && this->pair_stop.second == other.pair_stop.second);
    }
};

struct Bus {
    std::string name;
    std::vector<Stop*> stop_names;
};

struct Distance {
    PairStop stop_pair;
    int distance;
};

class DistanceHasher {
public:
    size_t operator()(const transport::catalogue::PairStop& pair_stop) const {
        return (std::hash<std::string>{} (pair_stop.pair_stop.first) + std::hash<std::string>{} (pair_stop.pair_stop.second));
    }
};

class TransportCatalogue {
public:
    void AddStop(const Stop &stop);

    void AddBus(const Bus &bus);

    void AddDistance(const Distance& distance);

    Stop* FindStop(std::string_view stop_name);

    std::string GetBusInfo(const std::string& bus_name) const;

    std::vector<std::string> GetBusesByStop(const std::string& stop_name) const;

private:
    std::deque<Stop> stops;
    std::deque<Bus> buses;

    std::unordered_map<std::string, Stop> stops_map;
    std::unordered_map<std::string, Bus> buses_map;
    std::unordered_map<std::string, std::set<std::string>> buses_stops_map;

    std::unordered_map<PairStop, int, DistanceHasher> stop_distance;

    int UniqueStops(std::vector<Stop*>) const;
    double ComputeDistanceStops(std::vector<Stop*>) const;
    double ComputeRoadDistance(std::vector<Stop*>) const;
};
}