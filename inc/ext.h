#pragma once

#ifndef EXT_H
#define EXT_H

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// A series of extensions for c and c++ functionality.

#ifdef ZALGO_EXT_IO
	#include <stdio.h>
	#ifdef ZALGO_EXT_SILENCE_PRINTF
		#define printf(...)
	#endif
	#ifdef ZALGO_EXT_CONTROL_PRINTF
		#define printf(...) ZALGO_EXT::conditional_printf(__VA_ARGS__)
	#endif
#endif

namespace ZALGO_EXT {
	// Set i to 0 to silence printf
	int (*conditional_printf) (const char*, ...) = printf;
	int silent_printf(const char* _, ...) { return 0; }
	void printf_verbosity(int verbose) {
		if (!verbose) conditional_printf = silent_printf;
		else conditional_printf = printf;
	}
};

// returns whether _str1_ exactly matches _str2_
bool streq(char* str1, char* str2)
{
	while (*str1 != 0 and *str2 != 0)
	{
		if (!(*str1 == *str2)) return 0;
		str1++; str2++;
	}
	return (*str1 == *str2);
}

// returns true if _str_ contains EXACTLY 1 or EXACTLY 0 characters (followed by implicit null terminator)
bool str1char(char* str)
{
	if (*str == 0) return true;
	else if (*(++str) == 0) return true;
	return false;
}

// returns position of first occurence of _c_ in _str_
// if _c_ not found in _str_, returns 0
int strhas(char* str, char c)
{
	int pos = 1;
	while (*str != 0)
	{
		if (*str == c) return pos;
		str++; pos++;
	}
	return 0;
}

char* printbin(int bits, char bitc)
{
	char* str = (char*) malloc(bitc + 1);
	str[bitc] = '\0';
	for (int i = 0; i < bitc; i++)
	{
		str[bitc - 1 - i] = '0' + (0 != (bits & (1 << i)));
	}
	return str;
}

int binint(char* n)
{
	int ret = 0;
	int i = 0;
	for (char* bit = n; *bit != '\0'; bit++)
	{
		if (*bit - '0') ret |= 1 << i;
		i++;
	}
	return ret;
}

template <class T>
inline T min(T a, T b) {
	return (a < b) ? a : b;
}

template <class T>
inline T max(T a, T b) {
	return (a > b) ? a : b;
}

template <class T>
struct dlnode { // Doubly linked node
	T value;
	dlnode* next = NULL;
	dlnode* prev = NULL;
};

template <class T>
struct List {

	dlnode<T>* first;
	dlnode<T>* last;
	int size;

	List() {
		first = NULL;
		last = NULL;
		size = 0;
	}
	
	int append(T val) {
		size++;
		dlnode<T>* add = new dlnode<T>;
		add->value = val;
		if (first == NULL) {
			first = add;
			last = add;
			return size;
		}
		last->next = add;
		add->prev = last;
		last = add;
		return size;
	}

	int prepend(T val) {
		size++;
		dlnode<T>* add = new dlnode<T>;
		add->value = val;
		if (last == NULL) {
			first = add;
			last = add;
			return size;
		}
		first->prev = add;
		add->next = first;
		first = add;
		return size;
	}

	void insert(T val, int idx) {
		if (idx < 0 or idx > size - 1) return;
		size++;
		dlnode<T>* add = new dlnode<T>;
		add->value = val;
		if (first == NULL) {
			first = add;
			last = add;
			return;
		}
		dlnode<T>* current = first;
		for (int i = 0; i < idx - 1; i++) {
			current = current->next;
		}
		dlnode<T>* following = current->next;
		current->next = add;
		add->prev = current;
		add->next = following;
		following->prev = add;
	}
	
	int remove(int idx) {
		if (idx < 0 or idx > size - 1) return -1;
		size--;
		dlnode<T>* current = first;
		for (int i = 0; i < idx; i++) {
			current = current->next;
		}
		T val = current->value;
		if (current->prev == NULL) {
			if (current->next == NULL) {
				first = NULL;
				last = NULL;
				free(current);
				return idx;
			}
			first = current->next;
			current->next->prev = NULL;
			free(current);
			return idx;
		}
		if (current->next == NULL) {
			if (current->prev == NULL) {
				first = NULL;
				last = NULL;
				free(current);
				return idx;
			}
			last = current->prev;
			current->prev->next = NULL;
			free(current);
			return idx;
		}
		current->prev->next = current->next;
		current->next->prev = current->prev;
		free(current);
		return size;
	}
	
	int find(T val) {
		if (size == 0) return -1;
		dlnode<T>* current = first;
		for (int idx = 0; current != NULL; idx++) {
			if (current->value == val) return idx;
			current = current->next;
		}
		return -1;
	}
	
	T get(int idx) {
		if (idx < 0 or idx > size - 1 or size == 0) return NULL;
		dlnode<T>* current = first;
		for (int i = 0; i < idx; i++) {
			if (current != NULL) current = current->next;
		}
		if (current == NULL) return NULL;
		return current->value;
	}
	
	T operator[](int idx)
	{
		return get(idx);
	}

};

template <class T>
struct Array {
	T* data;
	unsigned int size;
	Array() {
		data = NULL;
		size = 0;
	}
	// use an existing data block
	Array(T* ptr, int s) {
		data = ptr;
		size = s;
	}
	// create an empty array of given size
	void allocate(int capacity)
	{
		data = (T*) malloc(capacity * sizeof(T));
		if (data == NULL) return;
		size = capacity;
	}
	// completely overwrites array buffer with a given byte
	void fill(int value)
	{
		memset(data, value, size * sizeof(T));
	}
	// returns size of array data in bytes
	int blocksize()
	{
		return size * sizeof(T);
	}
	// returns -1 for not enough memory
	int append(T value) {
		T* attempt = (T*) realloc(data, sizeof(T) * (size + 1));
		if (!attempt) return -1;
		data = attempt;
		data[size] = value;
		size++;
		return size - 1;
	}
	// returns -1 for not enough memory
	int prepend(T value) {
		T* attempt = (T*) realloc(data, sizeof(T) * (size + 1));
		if (!attempt) return -1;
		data = attempt;
		memmove(data, data + 1, sizeof(T) * size);
		data[0] = value;
		size++;
		return 0;
	}
	// returns -1 for not enough memory
	int insert(T value, int idx) {
		T* attempt = (T*) realloc(data, sizeof(T) * (size + 1));
		if (!attempt) return -1;
		memmove(data + idx, data + idx + 1, size - idx);
		data[idx] = value;
		size++;
		return idx;
	}
	// returns new size of array
	int remove(int idx) {
		memmove(data + idx + 1, data + idx, size - 1 - idx);
		data = (T*) realloc(data, sizeof(T) * (size - 1));
		size--;
		return size;
	}
	// removes all elements and frees/nulls data pointer
	void empty()
	{
		free(data);
		data = 0x0;
		size = 0;	
	}
	// returns NULL for invalid index
	inline T & get(int idx) {
		return data[idx];
	}
	
	T & operator[](int idx)
	{
		return data[idx];
	}
	
	// does nothing for invalid index
	inline void set(int idx, T value) {
		if (idx < 0 or idx > size - 1) return;
		data[idx] = value;
	}
	// returns index of first matching value
	// -1 = not found
	// -2 = array empty
	int find(T value) {
		if (size < 1) return -2;
		for (int idx = 0; idx < size; idx++) {
			if (data[idx] == value) return idx;
		}
		return -1;
	}
	#ifdef ZALGO_EXT_IO
	void dump() {
		for (int i = 1; i < size; i++) {
			printf("index %i, value %i\n",i,get(i - 1));
		}
	}
	#endif
};

template <int size, class T>
struct Table
{
	T data[size];
	int next = 0;
	int last = 0;
	
	void add(T item)
	{
		data[next] = item;
		
	}
};

struct nenum: Array<char*>
{
	nenum() {}
	
	nenum(char** keys, int ct) 
	{
		int keybytes = sizeof(char*) * ct;
		allocate(ct);
		memcpy(data, keys, keybytes);
	}
	
	inline int operator[](char* key)
	{
		for (int i = 0; i < size; i++)
		{
			if (strcmp(key, data[i]) == 0) return i;
		}
		return -1;
	}
};

template <class T>
struct Dict
{
	Array<char*> keys;
	Array<T> values;
	
	void add(char* key, T value)
	{
		keys.append(key);
		values.append(value);
	}
	
	// removes first element with 'key'
	void remove(char* key)
	{
		for (int i = 0; i < keys.size; i++)
		{
			
		}
	}
	
	// returns index of first element matching 'key'
	// returns -1 if key not found
	int find(char* key)
	{
		for (int i = 0; i < keys.size; i++)
		{
			if (strcmp(keys[i], key) == 0) return i;
		}
		return -1;
	}
	
	// returns index of first element matching 'value'
	// returns -1 if value not found
	int find(T value)
	{
		for (int i = 0; i < values.size; i++)
		{
			if (values[i] == value) return i;
		}
		return -1;
	}
};

#endif