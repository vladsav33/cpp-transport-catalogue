#include "input_reader.h"
#include <iostream>
#include "stat_reader.h"

using namespace std;

int main() {
    transport::catalogue::TransportCatalogue catalogue;

    transport::input::InputReader reader;
    reader.ReadInput(cin);
    reader.ApplyCommands(catalogue);

    transport::stat::PrintInfo(catalogue, cin, cout);
}