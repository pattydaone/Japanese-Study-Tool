#include <iostream>
#include "CSVreader.h"
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <numeric>


int main() {
    std::vector<int> indices;
    indices.resize(15);

    if (indices.begin() == indices.end()) {std::cout << "Lol" << '\n';}

    std::iota(indices.begin(), indices.end(), 0);

    for (auto i : indices) {
    	std::cout << i << '\n';
    }
}
