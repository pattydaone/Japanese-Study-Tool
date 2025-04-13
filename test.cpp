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
    std::vector<int> indices { 1, 2, 3, 4, 5, 6 };
	std::cout << *(indices.begin()) << '\n';
	std::cout << *(indices.end()) << '\n';
	return 0;

}
