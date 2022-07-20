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
	base_container<T>* begin_container;
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

template <class T>
points_set<T>::points_set() {
	begin_container = nullptr;
}

template <class T>
points_set<T>::~points_set() {
	clear();
}

template <class T>
void points_set<T>::clear() {
	while (begin_container != nullptr) {
		auto next = begin_container->next;
		delete begin_container;
		begin_container = next;
	}

	size = 0;
}

template <class T>
void points_set<T>::push(T data) {
	if (try_get(data))
		return;
	else {
		if (begin_container == nullptr) {
			begin_container = new base_container<T>();
			begin_container->next = nullptr;
			begin_container->data = data;
		} else {
			auto container = new base_container<T>();
			container->data = data;
			container->next = nullptr;

			// TODO: need sort or not
			(*this)[size - 1]->next = container;
		}

		++size;
	}
}

template <class T>
bool points_set<T>::try_get(T data) {
	for (int i = 0; i < size; ++i) {
		if (((*this)[i])->data == data)
			return true;
	}

	return false;
}

template <class T>
int points_set<T>::get_size() {
	return size;
}

template <class T>
base_container<T>* points_set<T>::operator[](int id) {
	int i = 0;

	auto next = begin_container;
	while (next != nullptr && i != id) {
		next = next->next;
		++i;
	}

	if (next != nullptr)
		return next;
	else
		return nullptr;
}