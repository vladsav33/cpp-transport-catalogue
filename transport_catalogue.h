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

struct Bus {
    std::string name;
    std::vector<std::string_view> stop_names;
};

class TransportCatalogue {
public:
    void AddStop(const Stop &stop);

    void AddBus(const Bus &bus);

    std::optional<std::string> FindStop(std::string_view stop_name) const;

    std::string& FindBus(std::string& bus_name);

    std::string GetBusInfo(const std::string& bus_name) const;

    std::vector<std::string> GetBusesByStop(const std::string& stop_name) const;

private:
    std::deque<Stop> stops;
    std::deque<Bus> buses;

    std::unordered_map<std::string, Stop> stops_map;
    std::unordered_map<std::string, Bus> buses_map;
    std::unordered_map<std::string, std::set<std::string>> buses_stops_map;

    int UniqueStops(const std::vector<std::string_view>&) const;
    double ComputeDistanceStops(const std::vector<std::string_view>&) const;
};
}