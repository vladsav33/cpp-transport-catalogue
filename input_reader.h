#pragma once

#include "geo.h"
#include <string>
#include <string_view>
#include "transport_catalogue.h"
#include <vector>

namespace transport::input {

    struct CommandDescription {
        // Определяет, задана ли команда (поле command непустое)
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // Название команды
        std::string id;           // id маршрута или остановки
        std::string description;  // Параметры команды
    };

    class InputReader {
    public:
        /**
         * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
         */
        void ParseLine(std::string_view line);

        /**
         * Наполняет данными транспортный справочник, используя команды из commands_
         */
        void ApplyCommands(catalogue::TransportCatalogue& catalogue) const;

        void ReadInput(std::istream& in);

    private:
        std::vector<CommandDescription> commands_;
    };
}
