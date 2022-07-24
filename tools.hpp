#pragma once

#include <sstream>
#include <vector>

template <typename Out>
void Split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> Split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    Split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string GetDataFromTimestamp(const time_t rawtime)
{
    struct tm * dt;
    char buffer [30];

    dt = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H/%M/%S", dt);

    return std::string(buffer);
}
