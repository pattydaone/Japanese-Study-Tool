#ifndef TEXTREADER_H
#define TEXTREADER_H

#include <iostream>
#include <string>

inline std::istream& operator>>(std::istream& file, std::string& str)
{
    std::getline(file, str);
    return file;
}

#endif // TEXTREADER_H
