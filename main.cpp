#include <iostream>
#include "json_reader.h"
#include "graph.h"
#include "router.h"

using namespace std;

int main() {
    transport::catalogue::TransportCatalogue catalogue;

    json::JsonReader reader(move(json::Load(cin)));

    reader.ReadSettings();
    reader.ReadRouterSettings(catalogue);
    reader.ReadBaseRequest(catalogue);
    reader.ReadStatRequests(catalogue);

//    graph::DirectedWeightedGraph<double> routes_graph(5);
//    routes_graph.AddEdge({0, 0, 2.0});
//    routes_graph.AddEdge({0, 2, 3.0});
//    routes_graph.AddEdge({0, 3, 4.0});
//    routes_graph.AddEdge({3, 4, 5.0});
//
//    graph::Router<double> router(routes_graph);
//    auto result = router.BuildRoute(0, 4);
//    int a = 1;
}