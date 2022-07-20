#pragma once
#include <limits>
#include <cmath>
#include <list>

#include "../points_set/points_set.h"
#include "../points_set/point.h"

// Distance between points -> Lenght of vector
float distance(point& first, point& second) {
	float dist = 0.0f;
	point temp;
	
	temp.position.x = first.position.x - second.position.x;
	temp.position.y = first.position.y - second.position.y;
	dist = std::sqrt(temp.position.x * temp.position.x + temp.position.y * temp.position.y);

	return dist;
}

// Find point using average arithmetic point (faster)
int fast_algorithm(points_set<point>& set_of_points) {
	point average_point;
	average_point.position.x = 0.0f;
	average_point.position.y = 0.0f;

	if (set_of_points.get_size() > 0) {
		// search average point
		auto item = set_of_points[0];
		while (item != nullptr) {
			auto data = item->data;
			average_point.position.x += data.position.x;
			average_point.position.y += data.position.y;
			item = item->next;
		}

		// Calc average point
		average_point.position.x /= set_of_points.get_size();
		average_point.position.y /= set_of_points.get_size();
	
		// search point with lowest summ of distances
		float lowest_dist = (std::numeric_limits<float>::max)();
		int result_id = -1;
		int id = 0;
		item = set_of_points[0];
		while (item != nullptr) {
			auto dist = distance(average_point, item->data);
			if (dist < lowest_dist) {
				lowest_dist = dist;
				result_id = id;
			}
			item = item->next;
			++id;
		}

		// return point id
		return result_id;
	}

	return -1;
}

std::list<float> get_distances(points_set<point>& set_of_points) {
	auto item_external_loop = set_of_points[0];
	std::list<float> results;

	while (item_external_loop != nullptr) {
		auto data = item_external_loop->data;
		auto item_inner_loop = set_of_points[0];

		float result_dist = 0.0f;
		while (item_inner_loop != nullptr) {
			if (item_inner_loop->data.id != item_external_loop->data.id) {
				auto data = item_inner_loop->data;
				auto dist = distance(item_external_loop->data, item_inner_loop->data);
				result_dist += dist;
			}
			item_inner_loop = item_inner_loop->next;
		}

		results.push_back(result_dist);
		item_external_loop = item_external_loop->next;
	}

	return results;
}

// We calculate the sum of distances for each point (slower)
int slow_algorithm(points_set<point>& set_of_points) {
	float lowest_dist = (std::numeric_limits<float>::max)();
	auto distances = get_distances(set_of_points);

	int result_id = -1;
	int id = 0;
	for (auto& item : distances) {
		if (item < lowest_dist) {
			lowest_dist = item;
			result_id = id;
		}
		++id;
	}

	return result_id;
}