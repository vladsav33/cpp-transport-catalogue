#include <algorithm>
#include "map_renderer.h"
#include <map>
#include <ostream>
#include <unordered_set>
#include <set>

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

using namespace std;

void PrintMap(const map<string, Bus>& buses_map, RenderSetting& setting, std::ostream& out) {
    svg::Document doc;

    unordered_set<transport::geo::Coordinates, transport::geo::CoordinatesHasher> points;
    for (auto& [name, bus] : buses_map) {
        for (auto& stop : bus.stop_names) {
            points.insert(stop->coord);
        }
    }
    SphereProjector sphere(points.begin(), points.end(), setting.width, setting.height, setting.padding);

    for (auto& [name, bus] : buses_map) {
        svg::Polyline line;
        line.SetStrokeColor(setting.nextColor()).SetFillColor(svg::NoneColor).SetStrokeWidth(setting.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        for (auto& point : bus.stop_names) {
            line.AddPoint(sphere(point->coord));
        }
        doc.Add(line);
    }

    setting.reset();
    for (auto& [name, bus] : buses_map) {
        if (bus.stop_names.empty()) {
            continue;
        }
        svg::Text text1;
        svg::Text text2;
        svg::Color color = setting.nextColor();
        auto& point1 = bus.stop_names[0];
            text1.SetFontSize(setting.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                 .SetData(name).SetFillColor(setting.underlayer_color).SetStrokeColor(setting.underlayer_color)
                 .SetStrokeWidth(setting.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetPosition(sphere(point1->coord))
                 .SetOffset(svg::Point(setting.bus_label_offset.first, setting.bus_label_offset.second));
            text2.SetFontSize(setting.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                    .SetData(name).SetPosition(sphere(point1->coord)).SetFillColor(color)
                    .SetOffset(svg::Point(setting.bus_label_offset.first, setting.bus_label_offset.second));
            doc.Add(text1);
            doc.Add(text2);

        if (!bus.round_route && bus.stop_names.size() > 1
            && bus.stop_names[bus.stop_names.size() / 2]->name != bus.stop_names[0]->name) {
            auto& point2 = bus.stop_names[bus.stop_names.size() / 2];
            text1.SetFontSize(setting.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                    .SetData(name).SetFillColor(setting.underlayer_color).SetStrokeColor(setting.underlayer_color)
                    .SetStrokeWidth(setting.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetPosition(sphere(point2->coord))
                    .SetOffset(svg::Point(setting.bus_label_offset.first, setting.bus_label_offset.second));
            text2.SetFontSize(setting.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                    .SetData(name).SetPosition(sphere(point2->coord)).SetFillColor(color)
                    .SetOffset(svg::Point(setting.bus_label_offset.first, setting.bus_label_offset.second));
            doc.Add(text1);
            doc.Add(text2);
        }
    }

    auto cmp = [](Stop* a, Stop* b) {
        return a->name < b->name;
    };
    set<Stop*, decltype(cmp)> stops(cmp);
    for (auto& [name, bus] : buses_map) {
        for (auto& point : bus.stop_names) {
            stops.insert(point);
        }
    }

    for (auto& stop : stops) {
        svg::Circle circle;
        circle.SetCenter(svg::Point(sphere(stop->coord))).SetRadius(setting.stop_radius).SetFillColor("white");
        doc.Add(circle);
    }

    for (auto& stop : stops) {
        svg::Text text1;
        svg::Text text2;
        text1.SetPosition(sphere(stop->coord)).SetOffset(svg::Point(setting.stop_label_offset.first, setting.stop_label_offset.second))
             .SetFontSize(setting.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name)
             .SetFillColor(setting.underlayer_color).SetStrokeColor(setting.underlayer_color)
             .SetStrokeWidth(setting.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
             .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text2.SetPosition(sphere(stop->coord)).SetOffset(svg::Point(setting.stop_label_offset.first, setting.stop_label_offset.second))
                .SetFontSize(setting.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name)
                .SetFillColor("black");
        doc.Add(text1);
        doc.Add(text2);
    }
    doc.Render(out);
}

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}