#include <cassert>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include "transport_catalogue.h"

using namespace std;

namespace transport::catalogue {

    void TransportCatalogue::AddStop(Stop& stop) {
        stop.id = stops.size();
        stops.push_back(stop);
        stops_map[stop.name] = stops.back();
    }

    std::size_t TransportCatalogue::GetId(string stop_name) {
        assert(stops_map.count(stop_name) > 0);
        return stops_map.at(stop_name).id;
    }

    void TransportCatalogue::AddBus(const Bus& bus) {
        for (int i = 0; i < bus.stop_names.size(); ++i) {
            auto stop = bus.stop_names.at(i);
            if (buses_stops_map.count(string(stop->name))) {
                set<string>& buses_set = buses_stops_map.at(string(stop->name));
                buses_set.insert(bus.name);
            } else {
                set<string> buses_set;
                buses_set.insert(bus.name);
                buses_stops_map[string(stop->name)] = buses_set;
            }

            int distance = 0;
            int stops_count = 1;
            string prev_stop = stop->name;
            for (int j = i + 1; j < bus.stop_names.size(); ++j) {
                auto next_stop = bus.stop_names.at(j);
                if (stop_distance.count({pair(prev_stop, next_stop->name)}) > 0) {
                    distance += stop_distance.at({pair(prev_stop, next_stop->name)});
                } else {
                    distance += stop_distance.at({pair(next_stop->name, prev_stop)});
                }
                int edge = bus_graph_.AddEdge({GetId(stop->name), GetId(next_stop->name),
                        distance / (velocity_ * 1000 / 60) + wait_time_, bus.name, stop->name, stops_count++,
                       distance / (velocity_ * 1000 / 60)});
                edge_map[edge] = {stop->name, bus.name, stops_count++, distance / (velocity_ * 1000 / 60)};
                prev_stop = next_stop->name;
            }
        }
        buses.push_back(bus);
        buses_map[bus.name] = buses.back();
    }

    void TransportCatalogue::AddDistance(const Distance& distance) {
        stop_distance[distance.stop_pair] = distance.distance;
    }

    Stop* TransportCatalogue::FindStop(string_view stop_name) {
        if (stops_map.count(string(stop_name)) == 0) {
            return nullptr;
        }
        return &(stops_map.at(string(stop_name)));
    }

    optional<BusStat> TransportCatalogue::GetBusInfo(const string& bus_name) const {
        ostringstream str_out;
        BusStat bus_stat{};
        if (buses_map.count(bus_name) == 0) {
            return nullopt;
        } else {
            bus_stat.route_length = ComputeRoadDistance(buses_map.at(bus_name).stop_names);
            bus_stat.stop_count = buses_map.at(bus_name).stop_names.size();
            bus_stat.unique_stop_count = UniqueStops(buses_map.at(bus_name).stop_names);

            bus_stat.curvature =  bus_stat.route_length / ComputeDistanceStops(buses_map.at(bus_name).stop_names);
        }
        return bus_stat;
    }

    std::map<std::string, Bus> TransportCatalogue::GetRoutes() {
        return buses_map;
    }

    vector<vector<transport::geo::Coordinates>> TransportCatalogue::GetRouteCoordinates() {
        vector<vector<transport::geo::Coordinates>> routes;
        for (auto& [bus, value] : buses_map) {
            cout << bus << endl;
            vector<transport::geo::Coordinates> route;
            for (auto& stop : value.stop_names) {
                route.push_back(stop->coord);
            }
            routes.emplace_back(route);
        }
        return routes;
    }

    int TransportCatalogue::UniqueStops(const vector<Stop*> stop_names) const {
        set<string> unique;
        for (auto& stop : stop_names) {
            unique.insert(string(stop->name));
        }
        return unique.size();
    }

    double TransportCatalogue::ComputeDistanceStops(const vector<Stop*> stop_names) const {
        double distance = 0;
        auto prev_stop = stop_names[0];
        for (auto& stop : stop_names) {
            distance += geo::ComputeDistance(stops_map.at(string(prev_stop->name)).coord, stops_map.at(string(stop->name)).coord);
            prev_stop = stop;
        }
        return distance;
    }

    double TransportCatalogue::ComputeRoadDistance(const vector<Stop*> stop_names) const {
        double distance = 0;
        auto prev_stop = stop_names[0];
        Stop* stop;
        for (size_t i = 1; i < stop_names.size(); ++i) {
            stop = stop_names[i];
            if (stop_distance.count({pair<string, string>(prev_stop->name, stop->name)}) > 0) {
                distance += stop_distance.at({pair<string, string>(prev_stop->name, stop->name)});
            } else {
                assert(stop_distance.count({pair<string, string>(stop->name, prev_stop->name)}) > 0);
                distance += stop_distance.at({pair<string, string>(stop->name, prev_stop->name)});
            }
            prev_stop = stop;
        }
        return distance;
    }

    vector<string> TransportCatalogue::GetBusesByStop(const std::string& stop_name) const {
        vector<string> buses_by_stop;
        if (buses_stops_map.count(stop_name) == 0) {
            return buses_by_stop;
        }
        for (const auto& bus : buses_stops_map.at(stop_name)) {
            buses_by_stop.emplace_back(bus);
        }
        return buses_by_stop;
    }
}