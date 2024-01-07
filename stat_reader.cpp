#include "stat_reader.h"

using namespace std;

namespace transport::stat {

void ParseAndPrintStat(const catalogue::TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto first = request.find_first_not_of(' ');
    if (request[first] == 'B') {
        auto last = request.find_last_not_of(' ');
        auto next = request.find_first_not_of(' ', 3);
        auto str = request.substr(next, last - next + 1);
        output << transport_catalogue.GetBusInfo(string(str)) << "\r\n"s;
    }

    if (request[first] == 'S') {
        auto last = request.find_last_not_of(' ');
        auto next = request.find_first_not_of(' ', 4);
        auto str = request.substr(next, last - next + 1);
        output << transport_catalogue.GetBusesByStop(string(str)) << "\r\n"s;
    }
}
}