#include "json.h"

namespace json
{
class Builder;
class KeyContext;
class ArrayContext;
class DictContext;

class Builder {
public:
    KeyContext& Key(std::string);
    Builder& Value(Node);
    DictContext& StartDict();
    ArrayContext& StartArray();
    Builder& EndDict();
    Builder& EndArray();
    Node Build();
    std::vector<Node> nodes_;
private:
    int map_depth = 0;
    int array_depth = 0;
    bool key_ = false;
    bool new_element = true;

    std::vector<std::string> keys_;
    Node node_;
};

class DictContext {
public:
    DictContext(Builder& builder)
            : builder_(builder) {};
    KeyContext& Key(std::string str) {
        return builder_.Key(str);
    }
    Builder& EndDict() {
        return builder_.EndDict();
    }
private:
    Builder& builder_;
};

class ArrayContext {
public:
    ArrayContext(Builder& builder)
        : builder_(builder) {};
    ArrayContext& Value(Node node) {
        builder_.Value(std::move(node));
        return *this;
    }
    DictContext &StartDict() {
        return builder_.StartDict();
    }
    ArrayContext& StartArray() {
        return builder_.StartArray();
    }
    Builder &EndArray() {
        return builder_.EndArray();
    }
private:
    Builder& builder_;
};

class KeyContext {
public:
    KeyContext(Builder& builder)
        : builder_(builder) {};
    DictContext& Value(Node node) {
        builder_.Value(node);
        auto tmp = new DictContext(builder_);
        return *tmp;
    }
    DictContext& StartDict() {
        return builder_.StartDict();
    }
    ArrayContext& StartArray() {
        return builder_.StartArray();
    }
private:
    Builder& builder_;
};
}