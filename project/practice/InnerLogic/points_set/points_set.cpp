#include "points_set.h"

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
}

template <class T>
void points_set<T>::push(T data) {
	if (try_get(data))
		return;
	else {
		if (begin_container == nullptr) {
			begin_container = new base_container();
			begin_container->next = nullptr;
			begin_container->data = 
		}
		else {
			auto container = new base_container();
			container->data = data;
			container->next = nullptr;

			// TODO: need sort or not
			(*this)[size - 1]->next = container;
		}
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