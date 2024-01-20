#include <cassert>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include "transport_catalogue.h"

using namespace std;

namespace transport::catalogue {

void TransportCatalogue::AddStop(const Stop& stop) {
    stops.push_back(stop);
    stops_map[stop.name] = stops.back();
}

void TransportCatalogue::AddBus(const Bus& bus) {
    for (auto& stop : bus.stop_names) {
        if (buses_stops_map.count(string(stop->name))) {
            set<string>& buses_set = buses_stops_map.at(string(stop->name));
            buses_set.insert(bus.name);
        } else {
            set<string> buses_set;
            buses_set.insert(bus.name);
            buses_stops_map[string(stop->name)] = buses_set;
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

string TransportCatalogue::GetBusInfo(const string& bus_name) const {
    ostringstream str_out;
    if (buses_map.count(bus_name) == 0) {
        str_out << "Bus "s << bus_name << ": not found"s;
    } else {
        double road_distance = ComputeRoadDistance(buses_map.at(bus_name).stop_names);
        str_out << "Bus "s << bus_name << ": "s <<
                    buses_map.at(bus_name).stop_names.size() << " stops on route, "s <<
                    UniqueStops(buses_map.at(bus_name).stop_names) << " unique stops, "s <<
                    road_distance << " route length, "s <<
                    setprecision(6) << road_distance / ComputeDistanceStops(buses_map.at(bus_name).stop_names) << " curvature"s;
    }
    return str_out.str();
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
        distance += ComputeDistance(stops_map.at(string(prev_stop->name)).coord, stops_map.at(string(stop->name)).coord);
        prev_stop = stop;
    }
    return distance;
}

double TransportCatalogue::ComputeRoadDistance(const vector<Stop*> stop_names) const {
    double distance = 0;
    auto prev_stop = stop_names[0];
    Stop* stop;
    for (int i = 1; i < stop_names.size(); ++i) {
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