#include <iostream>
#include "json_reader.h"

using namespace std;

int main() {
    transport::catalogue::TransportCatalogue catalogue;

    json::JsonReader reader(move(json::Load(cin)));

    reader.ReadSettings();
    reader.ReadBaseRequest(catalogue);
    reader.ReadStatRequests(catalogue);
}