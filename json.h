#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

using Number = std::variant<int, double>;

namespace json {

    using namespace std::literals;

    class Node;
// Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        /* Реализуйте Node, используя std::variant */
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node();

        Node(nullptr_t);

        Node(int);

        Node(double);

        Node(Number);

        Node(bool);

        Node(Array);

        Node(Dict map);

        Node(std::string value);

        Array &AsArray();

        Dict &AsDict();

        int AsInt();

        double AsDouble();

        std::string &AsString();

        bool AsBool();

        Value &GetValue() {
            return value_;
        }

        bool IsInt() const;

        bool IsDouble() const;

        bool IsPureDouble() const;

        bool IsBool() const;

        bool IsString() const;

        bool IsNull() const;

        bool IsArray() const;

        bool IsDict() const;

        bool operator==(const Node &other) const;

        bool operator!=(const Node &other) const;

    private:
        Value value_;
    };

    void PrintValue(std::nullptr_t, std::ostream &);

    void PrintValue(int, std::ostream &);

    void PrintValue(double, std::ostream &);

    void PrintValue(std::string, std::ostream &);

    void PrintValue(bool, std::ostream &);

    void PrintValue(Array, std::ostream &);

    void PrintValue(Dict, std::ostream &);

    template<typename Value>
    void PrintValue(const Value &value, std::ostream &out);

    void PrintNode(Node node, std::ostream &out);

    Node LoadArray(std::istream &input);
    Node LoadDict(std::istream &input);
    std::string ExtractString(std::istream& input, const char open_ch, const char closing_ch);
    std::string LoadString(std::istream& input);
    Number LoadNumber(std::istream& input);
    Node LoadNode(std::istream &input);




    struct PrintContext {
        std::ostream &out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            out << std::string(indent, ' ');
        }
    };

    template<typename Value>
    void PrintValue(const Value &value, PrintContext &ctx) {
    }

    using Number = std::variant<int, double>;

// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
    std::string LoadString(std::istream& input);

    Number LoadNumber(std::istream& input);

    std::string ExtractString(std::istream& input, const char open_ch, const char closing_ch);
}  // namespace json
