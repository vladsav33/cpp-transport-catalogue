#include <cassert>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <iomanip>
#include "transport_catalogue.h"

using namespace std;

namespace transport::catalogue {

void TransportCatalogue::AddStop(const Stop& stop) {
    stops.push_back(stop);
    stops_map[stop.name] = stops.back();
}

void TransportCatalogue::AddBus(const Bus& bus) {
    for (auto& stop : bus.stop_names) {
        if (buses_stops_map.count(string(stop))) {
            set<string>& buses_set = buses_stops_map.at(string(stop));
            buses_set.insert(bus.name);
        } else {
            set<string> buses_set;
            buses_set.insert(bus.name);
            buses_stops_map[string(stop)] = buses_set;
        }
    }
    buses.push_back(bus);
    buses_map[bus.name] = buses.back();
}

optional<string> TransportCatalogue::FindStop(string_view stop_name) const {
    if (stops_map.count(string(stop_name)) == 0) {
        return {};
    }
    return stops_map.at(string(stop_name)).name;
}

string& TransportCatalogue::FindBus(string& bus_name) {
    assert(buses_map.count(bus_name) > 0);
    return bus_name;
}

string TransportCatalogue::GetBusInfo(const string& bus_name) const {
    ostringstream str_out;
    if (buses_map.count(bus_name) == 0) {
        str_out << "Bus "s << bus_name << ": not found"s;
    } else {
        str_out << "Bus "s << bus_name << ": " <<
                    buses_map.at(bus_name).stop_names.size() << " stops on route, "s <<
                    UniqueStops(buses_map.at(bus_name).stop_names) << " unique stops, "s <<
                    setprecision(6) << ComputeDistanceStops(buses_map.at(bus_name).stop_names) << " route length"s;
    }
    return str_out.str();
}

int TransportCatalogue::UniqueStops(const vector<string_view>& stop_names) const {
    set<string> unique;
    for (auto& stop : stop_names) {
        unique.insert(string(stop));
    }
    return unique.size();
}

double TransportCatalogue::ComputeDistanceStops(const vector<string_view>& stop_names) const {
    double distance = 0;
    auto prev_stop = stop_names[0];
    for (auto& stop : stop_names) {
        distance += ComputeDistance(stops_map.at(string(prev_stop)).coord, stops_map.at(string(stop)).coord);
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
        buses_by_stop.emplace_back(" "s + bus);
    }
    return buses_by_stop;
}
}