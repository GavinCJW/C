#define MYSQL_HOST "10.10.78.107"
#define MYSQL_USER "root"
#define MYSQL_PWD "xiaoniu66668888"
#define MYSQL_DBNAME "xcf_fund"
#define SAMPLE_NUMBER 365																				//样本数
#define FUND_NUMBER 61																					//基金数

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

int main(){
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
					cout   << fund_name[i]<< "data error";
					system("pause");
					exit(-1);
				}
				rate[i].resize(SAMPLE_NUMBER);
				for (int j = 0; j < SAMPLE_NUMBER; j++)
					rate[i][j] = stod(mtv.value[j][0]);
				retracement[i] = stod(mtv.value[0][0]);
			}else {
				cout << "load mysql error";
				system("pause");
				exit(-1);
			}
		}
	}

	MKWZ mkwz;
	auto ret = mkwz.getSpRtData(rate,retracement);

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
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	system("pause");
	return 0;
}
