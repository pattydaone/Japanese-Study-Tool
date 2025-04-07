#ifndef JOIN_H
#define JOIN_H 

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

void join(const std::vector<std::string>& vec, std::string& str) {
	for (std::string i : vec) {
		str += (i + ", ");
	}
	str.erase(str.find_last_of(", "));
}

#endif

