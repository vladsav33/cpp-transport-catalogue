#include "json_reader.h"
#include "map_renderer.h"
#include <sstream>

using namespace std;

namespace json {

svg::Color ReadNode(Node node);

Document::Document(Node root)
        : root_(move(root)) {
}

const Node &Document::GetRoot() const {
    return root_;
}

Document Load(istream &input) {
    Node node = LoadNode(input);
    return Document{node};
}

void Print(const Document &doc, std::ostream &output) {
    PrintNode(doc.GetRoot(), output);
}

bool Document::operator==(const Document &other) const {
    return root_ == other.root_;
}

bool Document::operator!=(const Document &other) const {
    return root_ != other.root_;
}

void JsonReader::SetDoc(Document&& document) {
    doc_ = document;
}

Document& JsonReader::GetDoc() {
    return doc_;
}

JsonReader::JsonReader(Document document)
    : doc_(std::move(document))
{
}

void JsonReader::ApplyCommands(transport::catalogue::TransportCatalogue& catalogue) {
    auto render = doc_.GetRoot().AsMap().at("render_settings").AsMap();
    RenderSetting setting;
    setting.width = render.at("width").AsDouble();
    setting.height = render.at("height").AsDouble();
    setting.padding = render.at("padding").AsDouble();
    setting.line_width = render.at("line_width").AsDouble();
    setting.stop_radius = render.at("stop_radius").AsDouble();
    setting.bus_label_font_size = render.at("bus_label_font_size").AsInt();
    setting.bus_label_offset.first = render.at("bus_label_offset").AsArray().at(0).AsDouble();
    setting.bus_label_offset.second = render.at("bus_label_offset").AsArray().at(1).AsDouble();
    setting.stop_label_font_size = render.at("stop_label_font_size").AsInt();
    setting.stop_label_offset.first = render.at("stop_label_offset").AsArray().at(0).AsDouble();
    setting.stop_label_offset.second = render.at("stop_label_offset").AsArray().at(1).AsDouble();
    setting.underlayer_color = ReadNode(render.at("underlayer_color"));
    setting.underlayer_width = render.at("underlayer_width").AsDouble();
    for (auto& color : render.at("color_palette").AsArray()) {
        setting.color_palette.push_back(ReadNode(color));
    }

    auto base = doc_.GetRoot().AsMap().at("base_requests").AsArray();
    for (Node request : base) {
        auto item = request.AsMap();
        if (item.at("type").AsString() == "Stop") {
            Stop stop = {item.at("name").AsString(),
                                            {item.at("latitude").AsDouble(),
                                            item.at("longitude").AsDouble()}};
            catalogue.AddStop(stop);
        }
    }

    for (Node request : base) {
        auto item = request.AsMap();
        if (item.at("type").AsString() == "Stop") {
            auto distance = item.at("road_distances").AsMap();
            string name = item.at("name").AsString();
            for (auto& [key, value] : distance) {
                transport::catalogue::Distance distance_elem;
                distance_elem.stop_pair.pair_stop.first = name;
                distance_elem.stop_pair.pair_stop.second = key;
                distance_elem.distance = value.AsInt();
                catalogue.AddDistance(distance_elem);
            }
        }
    }

    for (Node request : base) {
        auto item = request.AsMap();
        if (item.at("type").AsString() == "Bus") {
            string name = item.at("name").AsString();
            Bus bus = {name, vector<Stop*>(), false};
            for (auto& stop : item.at("stops").AsArray()) {
                bus.stop_names.push_back(catalogue.FindStop(stop.AsString()));
            }
            if (!item.at("is_roundtrip").AsBool()) {
                for (int ind = bus.stop_names.size() - 2; ind >= 0; --ind) {
                    bus.stop_names.push_back(bus.stop_names[ind]);
                }
                bus.round_route = false;
            } else {
                bus.round_route = true;
            }
            catalogue.AddBus(bus);
        }
    }

    auto stat = doc_.GetRoot().AsMap().at("stat_requests").AsArray();
    Array result;
    for (Node request : stat) {
        auto item = request.AsMap();

        if (item.at("type").AsString() == "Bus") {
            string name = item.at("name").AsString();
            int id = item.at("id").AsInt();
            Dict bus;
            bus["request_id"] = id;
            auto bus_stat = catalogue.GetBusInfo(name);
            if (bus_stat.has_value()) {
                bus["curvature"] = bus_stat.value().curvature;
                bus["route_length"] = bus_stat.value().route_length;
                bus["stop_count"] = bus_stat.value().stop_count;
                bus["unique_stop_count"] = bus_stat.value().unique_stop_count;
            } else {
                bus["error_message"] = "not found"s;
            }
            result.push_back(move(bus));
        }

        if (item.at("type").AsString() == "Stop") {
            string name = item.at("name").AsString();
            int id = item.at("id").AsInt();
            Dict buses;
            Array buses_list;
            for (auto bus : catalogue.GetBusesByStop(name)) {
                buses_list.push_back(Node(move(bus)));
            }
            if (buses_list.empty() && catalogue.FindStop(name) == nullptr) {
                buses["error_message"] = "not found"s;
            } else {
                buses["buses"] = buses_list;
            }
            buses["request_id"] = id;
            result.push_back(move(buses));
        }

        if (item.at("type").AsString() == "Map") {
            auto routes = catalogue.GetRoutes();
            Dict map;
            int id = item.at("id").AsInt();
            map["request_id"] = id;
            ostringstream out;
            PrintMap(routes, setting, out);
            map["map"] = out.str();
            result.push_back(move(map));
        }
    }
    PrintNode(Node(move(result)), cout);
}

svg::Color ReadNode(Node node) {
    if (node.IsString()) {
        return svg::Color(node.AsString());
    } else if (node.IsArray() && node.AsArray().size() == 3) {
        return svg::Rgb{(uint8_t) node.AsArray()[0].AsInt(),
                      (uint8_t) node.AsArray()[1].AsInt(),
                       (uint8_t) node.AsArray()[2].AsInt()};
    } else if (node.IsArray() && node.AsArray().size() == 4) {
        return svg::Rgba{(uint8_t) node.AsArray()[0].AsInt(),
                       (uint8_t) node.AsArray()[1].AsInt(),
                        (uint8_t) node.AsArray()[2].AsInt(),
                       node.AsArray()[3].AsDouble()};
    } else {
        return nullptr;
    }
}
} //namespace json