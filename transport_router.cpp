#include "transport_router.h"

TransportRouter::TransportRouter() {}

TransportRouter::TransportRouter(const graph::DirectedWeightedGraph<double>& graph)
    : router_(std::make_unique<graph::Router<double>>(graph))
{}

void TransportRouter::InitRouter() {
    router_ = std::make_unique<graph::Router<double>>(graph_);
}

graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() {
    return graph_;
}

std::optional<graph::Router<double>::RouteInfo> TransportRouter::BuildRoute(graph::VertexId from, graph::VertexId to) {
    return router_->BuildRoute(from, to);
}

