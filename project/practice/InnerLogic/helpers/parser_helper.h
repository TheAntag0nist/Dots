#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../points_set/point.h"

std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::vector<std::string> res;
    std::string token;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

point get_point(std::string line) {
	point new_point;

    auto cells = split(line, "\t");

    if (cells.size() > 1) {
        new_point.position.x = ::atof(cells[0].c_str());
        new_point.position.y = ::atof(cells[1].c_str());
    }

	return new_point;
}