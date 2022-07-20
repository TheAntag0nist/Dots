#pragma once
#include "imgui.h"


struct point {
	int id;
	ImVec2 position;

	bool operator==(point& second) {
		if (position.x == second.position.x &&
			position.y == second.position.y)
			return true;

		return false;
	}
};