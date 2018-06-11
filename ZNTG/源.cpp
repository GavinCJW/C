#define MYSQL_HOST "10.10.78.107"
#define MYSQL_USER "root"
#define MYSQL_PWD "xiaoniu66668888"
#define MYSQL_DBNAME "xcf_fund"
#define SAMPLE_NUMBER 92																				//样本数
#define FUND_NUMBER 61																						//基金数
#define SHORTLISTED_FUND_NUMBER 3																//入选基金数	
#define TEST_NUMBER 30																							//测试天数
#define TEST_FUND_NUMBER 3																				//测试基金数

#include <iostream>
#include <time.h>
#include <fstream>
#include <string>
#include "mysql.hpp"
#include "MKWZ.hpp"
using namespace std;

template<typename T>
inline std::istream &operator >> (std::istream &is, std::vector<T> &a) {
	auto rax = T();
	is >> rax;
	a.push_back(rax);
	return is;
}

void run();
void test();

int main(){
	//跑数据
	run();
	//测试数据
	//test();
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	system("pause");
	return 0;
}

void run() {
	vector<string> fund_name;
	vector<vector<double>> rate(FUND_NUMBER);
	vector<double> retracement(FUND_NUMBER);

	ifstream fin("config.txt");
	while (!fin.eof())
		fin >> fund_name;
	fin.close();

	if (fund_name.size() - 1 == FUND_NUMBER) {
		for (int i = 0; i < FUND_NUMBER; i++) {
			std::string query = "select fgr.dayGrowthRate ,pf.yearRetracement from FdayGrowthR fgr , p2_fundlist pf WHERE fgr.fundcode = '" + fund_name[i] + "' and pf.fundcode = '" + fund_name[i] + "' ORDER BY netdate DESC LIMIT " + std::to_string(SAMPLE_NUMBER) + ";";
			MySQL mysql;
			mysql.connect(MYSQL_HOST, MYSQL_USER, MYSQL_PWD, MYSQL_DBNAME);
			if (mysql.selectData(query.data())) {
				auto &mtv = mysql.get();
				if (mtv.number != SAMPLE_NUMBER) {
					cout << fund_name[i] << "data error";
					system("pause");
					exit(-1);
				}
				rate[i].resize(SAMPLE_NUMBER);
				for (int j = 0; j < SAMPLE_NUMBER; j++)
					rate[i][j] = stod(mtv.value[j][0]);
				retracement[i] = stod(mtv.value[0][1]);
			}
			else {
				cout << "load mysql error";
				system("pause");
				exit(-1);
			}
		}
	}

	MKWZ mkwz;
	auto ret = /*mkwz.getAllData(rate); */mkwz.getSpRtData(rate, retracement);

	/*ofstream fout("out.txt");
	auto num = ret.size();
	for (int i = 0; i < num; i++) {
	fout << i << ".\tEV：" << ret[i].e << "\tV：" << ret[i].v << "\tW：";
	for (int j = 0; j < SHORTLISTED_FUND_NUMBER; j++) {
	fout << ret[i].w[j] << "(" << fund_name[ret[i].p[j]] << ") , ";
	}
	fout << "\n";
	}
	fout.close();*/

	ofstream fout("out.txt");
	fout << ".\tEV：" << ret.e << "\tV：" << ret.v << "\tW：";
	for (int j = 0; j < SHORTLISTED_FUND_NUMBER; j++) {
		fout << ret.w[j] << "(" << fund_name[ret.p[j]] << ") , ";
	}
	fout << "\n";
	fout.close();
}

void test() {
	vector<string> fund_name;
	vector<vector<double>> rate(TEST_FUND_NUMBER);
	ifstream fin("test.txt");
	while (!fin.eof())
		fin >> fund_name;
	fin.close();
	if (fund_name.size() == TEST_FUND_NUMBER) {
		for (int i = 0; i < TEST_FUND_NUMBER; i++) {
			std::string query = "select fgr.dayGrowthRate from FdayGrowthR fgr WHERE fgr.fundcode = '" + fund_name[i] + "' and netdate > '2018-04-11' and netdate < '2018-05-12' ORDER BY netdate LIMIT " + std::to_string(TEST_NUMBER) + ";";
			MySQL mysql;
			mysql.connect(MYSQL_HOST, MYSQL_USER, MYSQL_PWD, MYSQL_DBNAME);
			if (mysql.selectData(query.data())) {
				auto &mtv = mysql.get();
				if (mtv.number != TEST_NUMBER) {
					cout << fund_name[i] << "data error";
					system("pause");
					exit(-1);
				}
				rate[i].resize(TEST_NUMBER);
				for (int j = 0; j < TEST_NUMBER; j++)
					rate[i][j] = stod(mtv.value[j][0]);
			}
			else {
				cout << "load mysql error";
				system("pause");
				exit(-1);
			}
		}
	}
	
	double w[3] = { 0.32,0.208345,0.471655 };
	ofstream fout("test_out.txt");
	CG<double> cg;
	std::vector<double> rates(TEST_NUMBER);
	for (int i = 0; i < TEST_FUND_NUMBER; i++) {
		auto ret = cg.get(rate[i]);
		fout << fund_name[i] << "：\n";
		for (int j = 0; j < TEST_NUMBER; j++) {
			fout << ret[j] << "\n";
			rates[j] += rate[i][j] * w[i];
			std::cout << rate[i][j] << "\n";
		}
		std::cout << "\n";
		fout << "\n";
	}
	
	auto ret = cg.get(rates);
	for (int j = 0; j < TEST_NUMBER; j++) {
		fout << ret[j] << "\n";
	}

	fout.close();
	

}
