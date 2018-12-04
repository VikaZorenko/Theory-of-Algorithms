#pragma once
#include <iostream>
#include <vector>
#include <chrono>

template<typename T>
class HashTable
{
private:
	int size;
	std::vector<std::pair<int, T*>> hashTable;
	int hashKeyFunc(int key, T &value) { 
		return this->size * ((key % 43) - int(value % 43));
	}
	int hashKeyFunc2(int key, T &value) { 
		return this->size * ((key % 43));
	}
public:
	HashTable(int size) {
		this->size = size;
		this->hashTable.resize(size, std::make_pair(-1, nullptr));
	};
	~HashTable() {};
	void add(int key, T &value) {
		int hashKey = this->hashKeyFunc(key);
		for (int i = 0; i < this->size; i++) {
			if (hashKey == this->size) hashKey = 0;
			if (this->hashTable.at(hashKey).second == nullptr) {
				this->hashTable.at(hashKey) = std::make_pair(key, &value);
				return;
			}
			hashKey++;
		}
		throw std::exception("Hash table is overcrowded");
	};

	T find (int key){
		int comparisons = 0;
		auto start = std::chrono::high_resolution_clock::now();
		int hashKey = this->hashKeyFunc(key);
		for (int i = 0; i < this->size; i++) {
			if (hashKey == this->size) hashKey = 0;
			if (this->hashTable.at(hashKey).second == nullptr)
				throw std::exception("There is not element with such key in the hash table");
			comparisons++;
			if (this->hashTable.at(hashKey).first == key) {
				auto end = std::chrono::high_resolution_clock::now();
				auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				std::cout << "Comparisons: " << comparisons << "\nSearch time: " << mcs.count() << " microsec\n";
				return *(this->hashTable.at(hashKey).second);
			}	
			hashKey++;
		}
		throw std::exception("There is not element with such key in the hash table");
	};
};

int main () {

}