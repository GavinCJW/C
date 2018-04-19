#pragma once

#include"cjw.hpp"
#include<thread>

class Thread {
private:
	std::vector<std::thread> threads;
	std::vector<int> heap;
public :
	Thread(int e) {
		heap.push_back(e);
	}
};
