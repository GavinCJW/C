#include <iostream>
#include <random>
#include "cjw.hpp"
#include "cjw_transcode.hpp"
#include "cjw_windows.hpp"
#include "cjw_http.hpp"
#include "cjw_mysql.hpp"
#include "cjw_des.hpp"

static int parse_number_string(const std::string &src) {
	static const char M_CVT[] = { '0','1','2','3','4','5','6','7','8','9', 'a','b','c','d','e','f','g','h','i','j'};
	int ret = 0;
	for (auto &i : src) {
		const auto r1x = i >= M_CVT[0] && i <= M_CVT[9] ? i - M_CVT[0] : i - M_CVT[10] + 10;
		ret = ret * 16 + r1x;
	}
	return std::move(ret);
}

static char* build_number_string(const int &src) {
	static const char M_CVT[] = { '0','1','2','3','4','5','6','7','8','9', 'a','b','c','d','e','f'};
	char *ret = new char[128];
	int iw = 0;
	auto rax = src;
	while (rax != 0) {
		ret[iw++] = M_CVT[rax % 10];
		rax /= 10;
	}
	ret[iw] = 0;
	for (int i = 0; i < iw + ~i; i++) {
		std::swap(ret[i], ret[iw + ~i]);
	}
	return std::move(ret);
}

std::vector<int> arrRdCC(int constant, int range) {
	std::vector<int> arr(range);
	std::random_device rd;
	std::mt19937 mt(rd());
	auto temp = constant;
	for (int i = 0; i < range; i++) {
		arr[i] = mt() % (constant / range) + 1;
		temp -= arr[i];
	}
	while (temp != 0) {
		for (int i = 0; i < range; i++) {
			if (arr[i] + temp < constant / 2) {
				arr[i] += temp;
				temp = 0;
			}
			else {
				auto t = mt() % (temp / range);
				arr[i] += t;
				temp -= t;
			}
		}
	}
	return arr;
}

int main() {

	auto tt = arrRdCC(1000, 3);
	for (int i = 0; i < 3; i++)
		std::cout << tt[i]<<"\t";

	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	system("pause");
	return 0;
}

