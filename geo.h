#pragma once

#include <cmath>

namespace transport::geo {

    struct Coordinates {
        double lat;
        double lng;
        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    class CoordinatesHasher {
    public:
        size_t operator() (const Coordinates& coord) const {
            return static_cast<size_t>(coord.lat * 37 + coord.lng);
        }
    };

    double ComputeDistance(Coordinates from, Coordinates to);
}