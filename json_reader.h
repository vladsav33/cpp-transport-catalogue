#pragma once

#include "json.h"
#include "map_renderer.h"
#include "router.h"
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
    void ReadRouterSettings(transport::catalogue::TransportCatalogue& catalogue);
    void ReadBaseRequest(transport::catalogue::TransportCatalogue& catalogue);
    void ReadStatRequests(transport::catalogue::TransportCatalogue& catalogue);

private:
    Document doc_;
    TransportRouter router_;
};

Document Load(std::istream &input);

void Print(Document &doc, std::ostream &output);

} //namespace json