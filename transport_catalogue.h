#pragma once
#include <deque>
#include "domain.h"
#include "geo.h"
#include "graph.h"
#include <optional>
#include <set>
#include <string>
#include "transport_router.h"
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
        void AddStop(Stop &stop);

        size_t GetId(std::string stop_name);

        void AddBus(const Bus &bus, TransportRouter& router);

        void AddDistance(const Distance& distance);

        Stop* FindStop(std::string_view stop_name);

        std::optional<BusStat> GetBusInfo(const std::string& bus_name) const;

        std::vector<std::string> GetBusesByStop(const std::string& stop_name) const;

        std::vector<std::vector<transport::geo::Coordinates>> GetRouteCoordinates();

        std::map<std::string, Bus> GetRoutes();

        void SetWait(int wait) {
            wait_time_ = wait;
        }

        void SetVelocity(int velocity) {
            velocity_ = velocity;
        }

        int GetWait() {
            return wait_time_;
        }

        double GetVelocity() {
            return velocity_;
        }

        std::map<int, EdgeDetails> GetEdgeMap() {
            return edge_map;
        }


    private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;

        std::unordered_map<std::string, Stop> stops_map;
        std::map<std::string, Bus> buses_map;
        std::unordered_map<std::string, std::set<std::string>> buses_stops_map;
        std::unordered_map<PairStop, int, DistanceHasher> stop_distance;
        std::map<int, EdgeDetails> edge_map;

        int UniqueStops(std::vector<Stop*>) const;
        double ComputeDistanceStops(std::vector<Stop*>) const;
        double ComputeRoadDistance(std::vector<Stop*>) const;

        int wait_time_;
        double velocity_;
    };
}