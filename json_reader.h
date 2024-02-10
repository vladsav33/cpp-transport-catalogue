#pragma once

#include "json.h"
#include <sstream>
#include "transport_catalogue.h"

namespace json {

class Document {
public:
    explicit Document(Node root);

    const Node &GetRoot() const;

    bool operator==(const Document &other) const;

    bool operator!=(const Document &other) const;

private:
    Node root_;
};

class JsonReader {
public:
    explicit JsonReader(Document);
    void SetDoc(Document&&);
    Document& GetDoc();
    void ApplyCommands(transport::catalogue::TransportCatalogue& catalogue);

private:
    Document doc_;
};

Document Load(std::istream &input);

void Print(const Document &doc, std::ostream &output);

} //namespace json