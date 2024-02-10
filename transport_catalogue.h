#pragma once
#include <deque>
#include "domain.h"
#include "geo.h"
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

namespace transport::catalogue {

    struct Distance {
        PairStop stop_pair;
        int distance;
    };

    class DistanceHasher {
    public:
        size_t operator()(const PairStop& pair_stop) const {
            return (std::hash<std::string>{} (pair_stop.pair_stop.first) + std::hash<std::string>{} (pair_stop.pair_stop.second));
        }
    };

    class TransportCatalogue {
    public:
        void AddStop(const Stop &stop);

        void AddBus(const Bus &bus);

        void AddDistance(const Distance& distance);

        Stop* FindStop(std::string_view stop_name);

        std::optional<BusStat> GetBusInfo(const std::string& bus_name) const;

        std::vector<std::string> GetBusesByStop(const std::string& stop_name) const;

        std::vector<std::vector<transport::geo::Coordinates>> GetRouteCoordinates();

        std::map<std::string, Bus> GetRoutes();

            private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;

        std::unordered_map<std::string, Stop> stops_map;
        std::map<std::string, Bus> buses_map;
        std::unordered_map<std::string, std::set<std::string>> buses_stops_map;

        std::unordered_map<PairStop, int, DistanceHasher> stop_distance;

        int UniqueStops(std::vector<Stop*>) const;
        double ComputeDistanceStops(std::vector<Stop*>) const;
        double ComputeRoadDistance(std::vector<Stop*>) const;
    };
}