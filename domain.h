#pragma once

#include "geo.h"
#include <string>
#include <vector>

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
struct Stop {
    std::string name;
    transport::geo::Coordinates coord;
    size_t id;
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
    bool round_route;
};

using BusPtr = Bus*;

struct BusStat {
    double curvature;
    int id;
    double route_length;
    int stop_count;
    int unique_stop_count;
};

struct EdgeDetails {
    std::string stop_name;
    std::string bus_name;
    int num_stops;
    double ride_time;
};