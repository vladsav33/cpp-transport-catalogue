#include <algorithm>
#include "map_renderer.h"
#include <ostream>
#include <unordered_set>
#include <set>

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

using namespace std;

void MapRenderer::InitSphere() {
    unordered_set<transport::geo::Coordinates, transport::geo::CoordinatesHasher> points;
    for (auto& [name, bus] : buses_map_) {
        for (auto& stop : bus.stop_names) {
            points.insert(stop->coord);
        }
    }
    sphere_ = SphereProjector(points.begin(), points.end(), settings_.width, settings_.height, settings_.padding);
}

void MapRenderer::PrintRoutes() {
    for (auto& [name, bus] : buses_map_) {
        svg::Polyline line;
        line.SetStrokeColor(settings_.nextColor()).SetFillColor(svg::NoneColor).SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        for (auto& point : bus.stop_names) {
            line.AddPoint(sphere_(point->coord));
        }
        doc_.Add(line);
    }
    settings_.reset();
}

void MapRenderer::PrintBusText() {
    for (auto& [name, bus] : buses_map_) {
        if (bus.stop_names.empty()) {
            continue;
        }
        svg::Text text1;
        svg::Text text2;
        svg::Color color = settings_.nextColor();
        auto& point1 = bus.stop_names[0];
        text1.SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                .SetData(name).SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetPosition(sphere_(point1->coord))
                .SetOffset(svg::Point(settings_.bus_label_offset.first, settings_.bus_label_offset.second));
        text2.SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                .SetData(name).SetPosition(sphere_(point1->coord)).SetFillColor(color)
                .SetOffset(svg::Point(settings_.bus_label_offset.first, settings_.bus_label_offset.second));
        doc_.Add(text1);
        doc_.Add(text2);

        if (!bus.round_route && bus.stop_names.size() > 1
            && bus.stop_names[bus.stop_names.size() / 2]->name != bus.stop_names[0]->name) {
            auto& point2 = bus.stop_names[bus.stop_names.size() / 2];
            text1.SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                    .SetData(name).SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color)
                    .SetStrokeWidth(settings_.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetPosition(sphere_(point2->coord))
                    .SetOffset(svg::Point(settings_.bus_label_offset.first, settings_.bus_label_offset.second));
            text2.SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold")
                    .SetData(name).SetPosition(sphere_(point2->coord)).SetFillColor(color)
                    .SetOffset(svg::Point(settings_.bus_label_offset.first, settings_.bus_label_offset.second));
            doc_.Add(text1);
            doc_.Add(text2);
        }
    }
}

void MapRenderer::PrintStops() {
    auto cmp = [](Stop* a, Stop* b) {
        return a->name < b->name;
    };
    set<Stop*, decltype(cmp)> stops(cmp);
    for (auto& [name, bus] : buses_map_) {
        for (auto& point : bus.stop_names) {
            stops.insert(point);
        }
    }

    for (auto& stop : stops) {
        svg::Circle circle;
        circle.SetCenter(svg::Point(sphere_(stop->coord))).SetRadius(settings_.stop_radius).SetFillColor("white");
        doc_.Add(circle);
    }

    for (auto& stop : stops) {
        svg::Text text1;
        svg::Text text2;
        text1.SetPosition(sphere_(stop->coord)).SetOffset(svg::Point(settings_.stop_label_offset.first, settings_.stop_label_offset.second))
                .SetFontSize(settings_.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name)
                .SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text2.SetPosition(sphere_(stop->coord)).SetOffset(svg::Point(settings_.stop_label_offset.first, settings_.stop_label_offset.second))
                .SetFontSize(settings_.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name)
                .SetFillColor("black");
        doc_.Add(text1);
        doc_.Add(text2);
    }
}

void MapRenderer::PrintMap() {
    doc_.Render(out_);
}

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}