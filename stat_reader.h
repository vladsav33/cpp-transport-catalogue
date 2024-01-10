#pragma once

#include <iosfwd>
#include <string_view>
#include "transport_catalogue.h"

namespace transport::stat {
void ParseAndPrintStat(const catalogue::TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);

void PrintInfo(catalogue::TransportCatalogue catalogue, std::istream& in, std::ostream& out);
}