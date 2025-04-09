#ifndef JOIN_H
#define JOIN_H 

#include <vector>
#include <string>

std::string join(const std::vector<std::string>& vec) {
	std::string str;
	for (std::string i : vec) {
		str += (i + ", ");
	}
	str.erase(str.find_last_of(", ") - 1);
	return str;
}

#endif

