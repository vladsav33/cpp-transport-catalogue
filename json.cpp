#include <cassert>
#include "json.h"

using namespace std;

std::string ProcessString(std::string& str) {
    std::string result;
    for (char i : str) {
        switch (i) {
            case '"' :
                result.append("\\\"");
                break;
            case '\\' :
                result.append("\\\\");
                break;
            case '\r' :
                result.append("\\r");
                break;
            case '\n' :
                result.append("\\n");
                break;
            case '\t' :
                result.append("\\t");
                break;
            default:
                result.push_back(i);
        }
    }
    return result;
}

namespace json {
Node LoadArray(istream &input) {
    Array result;

    char c;
    for (; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (c != ']') {
        throw ParsingError("No matching ]");
    }
    return Node(move(result));
}

Node LoadDict(istream &input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input);
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }
    return Node(move(result));
}

Node LoadNode(std::istream &input) {
    char c;
    input >> c;
    if (c == '[') {
        return LoadArray(input);
    } else if (c == ']') {
        throw ParsingError("");
    } else if (c == '{') {
        std::string str;
        str = ExtractString(input, '{', '}');
        std::istringstream s(str);
        return LoadDict(s);
    } else if (c == '}') {
        throw ParsingError("");
    } else if (c == '"') {
        return Node(LoadString(input));
    } else if (c == 'n') {
        char ch[4];
        input.read(ch, 4);
        std::string str{ch};
        if (str.find("ull") == str.npos ||  isalnum(input.peek())) {
            throw ParsingError("No null");
        }
        return Node();
    } else if (c == 't') {
        char ch[4];
        input.read(ch, 4);
        std::string str{ch};
        if (str.find("rue") == str.npos ||  isalnum(input.peek())) {
            throw ParsingError("No true");
        }
        return Node(true);
    } else if (c == 'f') {
        char ch[5];
        input.read(ch, 5);
        std::string str{ch};
        if (str.find("alse") == str.npos ||  isalnum(input.peek())) {
            throw ParsingError("No false");
        }
        return Node(false);
    } else {
        input.putback(c);
        return Node(LoadNumber(input));
    }
}

Node::Node()
        : value_(nullptr) {
}

Node::Node(std::nullptr_t)
        : value_(nullptr) {
}

Node::Node(double value)
        : value_(value) {
}

Node::Node(Number num) {
    if (holds_alternative<double>(num)) {
        value_ = get<double>(num);
    } else {
        value_ = get<int>(num);
    }
}

Node::Node(bool value)
        : value_(move(value)) {
}

Node::Node(Array array)
        : value_(move(array)) {
}

Node::Node(Dict map)
        : value_(move(map)) {
}

Node::Node(int value)
        : value_(value) {
}

Node::Node(string value)
        : value_(move(value)) {
    if (get<string>(value_) == "null") {
        value_ = nullptr;
    }
}

const Array &Node::AsArray() const {
    if (!holds_alternative<Array>(value_)) {
        throw logic_error("No int");
    }
    return get<Array>(value_);
}

const Dict &Node::AsMap() const {
    if (!holds_alternative<Dict>(value_)) {
        throw logic_error("No Map");
    }
    return get<Dict>(value_);
}

int Node::AsInt() const {
    if (!holds_alternative<int>(value_)) {
        throw logic_error("No int");
    }
    return get<int>(value_);
}

double Node::AsDouble() const {
    if (!holds_alternative<int>(value_) && !holds_alternative<double>(value_)) {
        throw logic_error("No double");
    }
    if (IsPureDouble()) {
        return get<double>(value_);
    } else {
        return get<int>(value_);
    }
}

const string &Node::AsString() const {
    if (!holds_alternative<string>(value_)) {
        throw logic_error("No string");
    }
    return get<string>(value_);
}

bool Node::AsBool() const {
    if (!holds_alternative<bool>(value_)) {
        throw logic_error("No bool");
    }
    return get<bool>(value_);
}



// Printing nodes
void PrintValue(std::nullptr_t, std::ostream &out) {
    out << "null"sv;
}

void PrintValue(int value, std::ostream &out) {
    out << value;
}

void PrintValue(double value, std::ostream &out) {
    out << value;
}

void PrintValue(string value, std::ostream &out) {
    out << "\""sv << ProcessString(value) << "\""sv;
}

void PrintValue(bool value, std::ostream &out) {
    if (value) {
        out << "true";
    } else {
        out << "false";
    }
}

void PrintValue(Array value, std::ostream &out) {
    out << "[";
    bool first = true;
    for (auto &val : value) {
        if (!first) {
            out << "," << endl;
        }
        PrintNode(Node(val), out);
        first = false;
    }
    out << "]";
}

void PrintValue(Dict value, std::ostream &out) {
    out << "{";
    bool first = true;
    for (auto[key, val] : value) {
        if (!first) {
            out << ", ";
        }
        first = false;

        if (key == "null") {
            out << "\"";
        }
        PrintNode(key, out);
        if (key == "null") {
            out << "\"";
        }
        out << ": ";
        PrintNode(val, out);
    }
    out << "}";
}

template<typename Value>
void PrintValue(const Value &value, std::ostream &out) {
}

void PrintNode(const Node &node, std::ostream &out) {
    std::visit([&out](const auto &value) { PrintValue(value, out); },
               node.GetValue());
}


bool Node::IsInt() const {
    return holds_alternative<int>(value_);
}

bool Node::IsDouble() const {
    return holds_alternative<double>(value_) || holds_alternative<int>(value_);
}

bool Node::IsPureDouble() const {
    return holds_alternative<double>(value_);
}

bool Node::IsBool() const {
    return holds_alternative<bool>(value_);
}

bool Node::IsString() const {
    return holds_alternative<string>(value_);
}

bool Node::IsNull() const {
    return holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return holds_alternative<Dict>(value_);
}

bool Node::operator==(const Node &other) const {
    return value_ == other.value_;
}

bool Node::operator!=(const Node &other) const {
    return value_ != other.value_;
}



Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

std::string LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return s;
}

std::string ExtractString(std::istream& input, const char open_ch, const char closing_ch) {
    int depth = 0;
    string result;
    char ch;
    while (true) {
        if (input.eof()) {
            throw ParsingError("No closing symbol");
        }
        ch = input.get();
        result.push_back(ch);
        if (ch == open_ch) {
            depth++;
        } else if (ch == closing_ch) {
            if (depth == 0) {
                return result;
            } else {
                depth--;
            }
        }
    }
}
}  // namespace json