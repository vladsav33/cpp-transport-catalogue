#pragma once

#include "json.h"
#include "map_renderer.h"
#include <sstream>
#include "transport_catalogue.h"

namespace json {

class Document {
public:
    explicit Document(Node root);

    Node &GetRoot();

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
    RenderSettings ReadSettings();
    void ReadBaseRequest(transport::catalogue::TransportCatalogue& catalogue);
    void ReadStatRequests(transport::catalogue::TransportCatalogue& catalogue);

private:
    Document doc_;
};

Document Load(std::istream &input);

void Print(Document &doc, std::ostream &output);

} //namespace json