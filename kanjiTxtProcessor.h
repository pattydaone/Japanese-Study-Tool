#ifndef KANJITXTPROCESSOR_H
#define KANJITXTPROCESSOR_H

#include <fstream>
#include <unordered_map>
#include <string>
#include <wx/gdicmn.h>
#include <vector>

inline std::ostream& operator<<(std::ostream& out, const wxPoint& point) {
    out << point.x << ", " << point.y;
    return out;
}

class Processor {
    using Lines = std::vector<std::vector<wxPoint>>;

    static void split(std::string& line, wxPoint& point) {
        std::size_t pos { line.find(", ", 0) };
        point.x = std::stoi(line.substr(0, pos));
        line.erase(0, pos + 2);
        point.y = std::stoi(line);
    }

    static void extractLine(std::ifstream& file, std::vector<wxPoint>& drawingLine) {
        std::string line;
        while (std::getline(file, line)) {
            if (line == '\0') return;
            drawingLine.push_back(wxPoint());
            split(line, drawingLine.back());
        }
    }

    static void extractDrawing(std::ifstream& file, Lines& drawing) {
        std::string line;
        while (std::getline(file, line)) {
            if (line == "\0\0") return;
            drawing.push_back(std::vector<wxPoint>());
            extractLine(file, drawing.back());
        }
    }

public:
    Processor() = delete;

    static void encodeToTxt(const std::string& path, const std::unordered_map<std::string, Lines>& drawings) {
        std::ofstream file { path };
        for (const auto& [key, value] : drawings) {
            file << key << '\n';
            for (auto i : value) {
                for (auto point : i) {
                    file << point << '\n';
                }
                file << '\0' << '\n';
            }
            file << '\0' << '\0' << '\n';
        }
    }

    static void decodeFromTxt(std::unordered_map<std::string, Lines>& map, const std::string& path) {
        std::ifstream file { path };
        std::string line;
        std::string kanji;
        while (std::getline(file, line)) {
            map[line] = Lines();
            extractDrawing(file, map[line]);
        }
    }
};

#endif
