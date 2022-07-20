#pragma once
#include "point.h"

template <class T>
struct base_container {
	T data;
	base_container<T>* next;
};

template <class T>
class points_set {
private:
	base_container* begin_container;
	int size;

public:
	points_set();
	~points_set();

	void clear();
	
	int get_size();
	void push(T data);
	bool try_get(T data);
	base_container<T>* operator[](int id);
};