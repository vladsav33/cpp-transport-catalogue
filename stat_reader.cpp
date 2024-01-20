#include <sstream>
#include "stat_reader.h"

using namespace std;

namespace transport::stat {

void ParseAndPrintStat(catalogue::TransportCatalogue& transport_catalogue, std::string_view request,
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
        output << "Stop "s << str << ":"s;
        if (transport_catalogue.FindStop(str) == nullptr) {
            output << " not found"s << "\r\n"s;
            return;
        }
        auto bus_by_stop = transport_catalogue.GetBusesByStop(string(str));
        if (!bus_by_stop.empty()) {
            output << " buses"s;
            for (auto bus : bus_by_stop) {
                output << " "s << bus;
            }
        } else {
            output << " no buses";
        }
        output << "\r\n"s;
    }
}

void PrintInfo(catalogue::TransportCatalogue& catalogue, istream& in, ostream& out) {
    int stat_request_count;
    in >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(in, line);
        transport::stat::ParseAndPrintStat(catalogue, line, out);
    }
}
}