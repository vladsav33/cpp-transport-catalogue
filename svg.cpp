#include <cmath>
#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<< (std::ostream& out, Color color) {
        std::visit(PrintColor{out}, color);
        return out;
    }

    std::ostream& operator<< (std::ostream& out, StrokeLineCap lineCap) {
        switch(lineCap) {
            case StrokeLineCap::BUTT:
                out << "butt";
                break;
            case StrokeLineCap::ROUND:
                out << "round";
                break;
            case StrokeLineCap::SQUARE:
                out << "square";
                break;
        }
        return out;
    }
//
    std::ostream& operator<< (std::ostream& out, StrokeLineJoin lineJoin) {
        switch (lineJoin) {
            case StrokeLineJoin::ARCS:
                out << "arcs";
                break;
            case StrokeLineJoin::BEVEL:
                out << "bevel";
                break;
            case StrokeLineJoin::MITER:
                out << "miter";
                break;
            case StrokeLineJoin::MITER_CLIP:
                out << "miter-clip";
                break;
            case StrokeLineJoin::ROUND:
                out << "round";
                break;
        }
        return out;
    }
//
    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }


    void Document::Render(std::ostream& out) const {
        RenderContext context(out);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        for (auto& obj : objects_) {
            obj->Render(context);
        }
        out << "</svg>" << std::endl;
    }

// ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "  <circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point) {
        line_.emplace_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto& out = context.out;
        out << "  <polyline points=\""sv;
        bool first_pair = true;
        for (auto point : line_) {
            if (!first_pair) {
                out << " "sv;
            }
            first_pair = false;
            out << ""sv << point.x << ","sv << point.y;
        }
        out << "\"";
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        weight_ = std::move(font_weight);
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    static std::string StringProcess(std::string str) {
        auto it = str.find("&");
        while (it != str.npos) {
            str = str.replace(it, 1, "&amp;");
            it = str.find("&", it + 5);
        }

        it = str.find("\"");
        while (it != str.npos) {
            str = str.replace(it, 1, "&quot;");
            it = str.find("\"", it + 6);
        }

        it = str.find("\'");
        while (it != str.npos) {
            str = str.replace(it, 1, "&apos;");
            it = str.find("\'", it + 6);
        }

        it = str.find("<");
        while (it != str.npos) {
            str = str.replace(it, 1, "&lt;");
            it = str.find("<", it + 4);
        }

        it = str.find(">");
        while (it != str.npos) {
            str = str.replace(it, 1, "&gt;");
            it = str.find(">", it + 4);
        }

        return str;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "  <text x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" "sv << "dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!weight_.empty()) {
            out << " font-weight=\""sv << weight_ << "\""sv;
        }
        RenderAttrs(context.out);
        out << ">";
        std::string data = StringProcess(data_);
        out << data;
        out << "</text>"sv;
    }



    // Прочие данные и методы, необходимые для реализации элемента <text>

}  // namespace svg


