#include "graph.h"
#include <memory>
#include "router.h"

static int BUS_STOPS_MAX = 100;

class TransportRouter {
public:
    TransportRouter();
    TransportRouter(const graph::DirectedWeightedGraph<double>& graph);

    void InitRouter();

    graph::DirectedWeightedGraph<double>& GetGraph();

    std::optional<graph::Router<double>::RouteInfo> BuildRoute(graph::VertexId from, graph::VertexId to);

private:
    std::unique_ptr<graph::Router<double>> router_;
    graph::DirectedWeightedGraph<double> graph_ = graph::DirectedWeightedGraph<double>(BUS_STOPS_MAX);
};