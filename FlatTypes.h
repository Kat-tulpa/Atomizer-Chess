#pragma once
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <iostream>

/*
	About FlatTypes.h
	-This file is meant to be a collection of more flat and specialized data containers
	-This is in the format of a header only lib, just include wherever you'd like
*/

/*
	About FlatMiniString
	-An intentionally flat indirectionless string abstraction meant to store
	very small strings
	Requirements
	-maxCapacity is set within ranges of 0 - 255
	-If allocating via malloc(), call init to avoid UB.
	-Expects sizeof(char) == sizeof(uint8_t);
	Usage
	-Short usage example...
		```
		FlatMiniString<64> miniStr;
		miniStr.push_back("hello world!");
		printf(miniStr[4]);  //prints o
		```
	-In order to enable bounds checking for debugging purposes define
	#define FLAT_TYPES_GUARD before including this file.
*/

template<uint8_t maxCapacity>
class FlatMiniString {
	//First element is count, last element is buffer NULL, note getData()
	char data[(uint16_t)maxCapacity + 2] = { 0 };
public:
	void init() {
		memset(this, 0, sizeof(*this));
	}
	inline char* getData() {
		return &data[1];
	}
	inline uint8_t getCount() const {
		return (uint8_t)data[0];
	}
	inline const char* c_str() const {
		return (const char*)&data[1];
	}
	inline char operator[](uint8_t i) const {
		#ifdef FLAT_TYPES_GUARD
		if (i >= maxCapacity) {
			std::cout << "Error: FlatMiniString::[] input index is not in bounds, index is: " << i 
				std::endl << "Last valid index is: " << maxCapacity - 1 << std::endl;
			throw;
		}
		#endif
		char* buff = getData();
		return buff[i];
	}
	inline char at(uint8_t i) const {
		if (i >= maxCapacity) {
			std::cout << "Error: FlatMiniString::at() input index is not in bounds, index is: " << i
				std::endl << "Last valid index is: " << maxCapacity - 1 << std::endl;
			throw;
		}
		char* buff = getData();
		return buff[i];
	}
	inline void append(char c) {
		char* buff = getData();
		uint8_t count = getCount();
		#ifdef FLAT_TYPES_GUARD
		if (count >= maxCapacity - 1) {
			std::cout << "Error: FlatMiniString::push_back(char) max length already reached"
				<< std::endl << "max capcity is: " << maxCapacity;
			throw;
		}
		#endif
		buff[count] = c;
		_setCount(count+1);
	}
	inline void append(const char* str) {
		size_t len = strlen(str);
		char* buff = getData();
		uint8_t count = getCount();
		#ifdef FLAT_TYPES_GUARD
		uint8_t spaceLeft = maxCapacity - count;
		if (len > spaceLeft) {
			std::cout << "Error: FlatMiniString::push_back(const char*) input string too big"
				<< std::endl << "max capacity is: " << maxCapacity
				<< std::endl << "space left is: " << spaceLeft
				<< std::endl << "input string length is: ";
			throw;
		}
		#endif
		memcpy(&buff[count], str, len);
		count += len;
		_setCount(count);
	}
	inline void append(const std::string& str) {
		size_t len = str.size();
		char* buff = getData();
		uint8_t count = getCount();
		#ifdef FLAT_TYPES_GUARD
		uint8_t spaceLeft = maxCapacity - count;
		if (len > spaceLeft) {
			std::cout << "Error: FlatMiniString::push_back(const std::string&) input string too big"
				<< std::endl << "max capacity is: " << maxCapacity
				<< std::endl << "space left is: " << spaceLeft
				<< std::endl << "input string length is: ";
			throw;
		}
		#endif
		memcpy(&buff[count], str, len);
		count += len;
		_setCount(count);
	}
	inline void append(const FlatMiniString& str) {
		size_t len = str.getCount();
		char* buff = getData();
		uint8_t count = getCount();
		#ifdef FLAT_TYPES_GUARD
		uint8_t spaceLeft = maxCapacity - count;
		if (len > spaceLeft) {
			std::cout << "Error: FlatMiniString::push_back(const FlatMiniString&) input string too big"
				<< std::endl << "max capacity is: " << maxCapacity
				<< std::endl << "space left is: " << spaceLeft
				<< std::endl << "input string length is: ";
			throw;
		}
		#endif
		memcpy(&buff[count], str, len);
		count += len;
		_setCount(count);
	}
private:
	inline void _setCount(uint8_t newCount) {
		uint8_t* countPtr = (uint8_t*)&data[0];
		*countPtr = newCount;
	}
};