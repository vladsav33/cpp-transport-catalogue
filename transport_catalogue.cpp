#include <cassert>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <iomanip>
#include "transport_catalogue.h"

using namespace std;

namespace transport::catalogue {

void TransportCatalogue::AddStop(Stop& stop) {
    stops.push_back(stop);
    stops_map[stop.name] = stops.back();
}

void TransportCatalogue::AddBus(Bus& bus) {
    for (auto& stop : bus.stop_names) {
        stop = FindStop(stop);
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

std::string& TransportCatalogue::FindStop(std::string_view stop_name) {
    assert(stops_map.count(string(stop_name)) > 0);
    return stops_map.at(string(stop_name)).name;
}

std::string& TransportCatalogue::FindBus(std::string& bus_name) {
    assert(buses_map.count(bus_name) > 0);
    return bus_name;
}

std::string TransportCatalogue::GetBusInfo(const std::string& bus_name) const {
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

std::string TransportCatalogue::GetBusesByStop(const std::string& stop_name) const {
    ostringstream str_out;

    str_out << "Stop "s << stop_name << ":"s;
    if (stops_map.count(stop_name) == 0) {
        str_out << " not found";
        return str_out.str();
    }
    if (buses_stops_map.count(stop_name) == 0) {
        str_out << " no buses";
        return str_out.str();
    }

    str_out << " buses";
    for (const auto& bus : buses_stops_map.at(stop_name)) {
        str_out << " "s << bus;
    }
    return str_out.str();
}
}