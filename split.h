#ifndef SPLIT_H
#define SPLIT_H

#include <vector>
#include <string_view>

void split(std::string_view str, std::vector<std::string>& vec) {
    std::size_t pos { 0 };

    while ((pos = str.find("｜｜", 0)) != std::string_view::npos) {
        vec.emplace_back(str.substr(0, pos));
        str.remove_prefix(pos + 6);
    }
    if (str.back() == '\r') { str.remove_suffix(1); }
    vec.emplace_back(str);
}

#endif // SPLIT_H
