#define _USE_MATH_DEFINES
#include <cmath>
#include "geo.h"

namespace transport::geo {

const double EARTH_RADIUS = 6371000;

double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * EARTH_RADIUS;
}

}  // namespace transport::geo