#include <iostream>
#include "json_builder.h"

using namespace std;

namespace json
{
KeyContext& Builder::Key(std::string str) {
    if (!nodes_.back().IsDict() || map_depth == 0 || key_) {
        throw logic_error("No Dict but Key found");
    }
    key_ = true;

    keys_.emplace_back(move(str));
    new_element = true;
    auto context = new KeyContext(*this);
    return *context;
}

Builder& Builder::Value(Node val) {
    if (!new_element) {
        throw logic_error("New element is not expected");
    }
    if (!nodes_.empty() && nodes_.back().IsArray()) {
        nodes_.back().AsArray().emplace_back(move(val));
        new_element = true;
    } else if (!nodes_.empty() && nodes_.back().IsDict()) {
        nodes_.back().AsDict()[keys_.back()] = val;
        keys_.pop_back();
        key_ = false;
        new_element = false;
    } else {
        node_ = Node(val);
        nodes_.push_back(move(node_));
        new_element = false;
    }
    return *this;
}

DictContext& Builder::StartDict() {
    if (!new_element) {
        throw logic_error("New Dict is not expected");
    }
    Dict dict;
    Node node = Node(move(dict));
    nodes_.emplace_back(move(node));
    map_depth++;
    key_ = false;
    new_element = false;
    auto context = new DictContext(*this);
    return *context;
}

ArrayContext& Builder::StartArray() {
    if (!new_element) {
        throw logic_error("New Array is not expected");
    }
    Array array;
    Node node = Node(move(array));
    nodes_.emplace_back(move(node));
    array_depth++;
    key_ = false;
    auto context = new ArrayContext(*this);
    return *context;
}

Builder& Builder::EndDict() {
    if (map_depth == 0) {
        throw logic_error("Map depth is wrong");
    }
    if (!nodes_.back().IsDict()) {
        throw logic_error("No StartDict found");
    }
    if (nodes_.size() > 1 && nodes_[nodes_.size() - 2].IsDict()) {
        nodes_[nodes_.size() - 2].AsDict()[keys_.back()] = nodes_.back();
        keys_.pop_back();
        nodes_.pop_back();
    } else if (nodes_.size() > 1 && nodes_[nodes_.size() - 2].IsArray()) {
        nodes_[nodes_.size() - 2].AsArray().emplace_back(nodes_.back());
        nodes_.pop_back();
        new_element = true;
    }
    map_depth--;
    return *this;
}

Builder& Builder::EndArray() {
    if (array_depth == 0) {
        throw logic_error("");
    }
    if (!nodes_.back().IsArray()) {
        throw logic_error("Last node is not Array");
    }
    if (nodes_.size() > 1 && nodes_[nodes_.size() - 2].IsArray()) {
        nodes_[nodes_.size() - 2].AsArray().emplace_back(nodes_.back());
        nodes_.pop_back();
    } else if (nodes_.size() > 1 && nodes_[nodes_.size() - 2].IsDict()) {
        nodes_[nodes_.size() - 2].AsDict()[keys_.back()] = nodes_.back();
        keys_.pop_back();
        nodes_.pop_back();

    }
    array_depth--;
    return *this;
}

Node Builder::Build() {
    if (array_depth != 0 || map_depth != 0 || nodes_.size() != 1) {
        throw logic_error("Array or Map is not completed");
    }
    if (nodes_.empty()) {
        return Node();
    }
    return nodes_[0];
}
}