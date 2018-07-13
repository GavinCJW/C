#include <iostream>
#include "cjw_des.hpp"
#include "cjw_sqllite.hpp"

class A {
public:
	A() {
		std::cout << "A";
		this->~A();
	}
	~A() {
		std::cout << "B";
	}
};

template<class T>
bool IS_TYPE(T key, std::vector<T> val) {
	for each (auto var in val) {
		if (key == var)
			return true;
	}
	return false;
};


int main() {
	
	/*SqlLite sl("test.db");

	char * sql = "CREATE TABLE COMPANY("  \
		"ID INT PRIMARY KEY     NOT NULL," \
		"NAME           TEXT    NOT NULL," \
		"AGE            INT     NOT NULL," \
		"ADDRESS        CHAR(50)," \
		"SALARY         REAL );";

	sl.WriteData(sql);

	sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		"VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		"VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
		"VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
		"VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

	sl.WriteData(sql);

	sql = "SELECT * from COMPANY";

	auto ret = *sl.SelectData(sql);

	for (auto r : ret.value) {
		for(auto i : r) 
			std::cout << i.first << " : " << i.second << std::endl;

		std::cout << std::endl;
	}*/

	auto fun = [](int key , std::vector<int> val) {
		for each (auto var in val){
			if (key == var)
				return true;
		}
		return false;
	};
	std::cout<<(IS_TYPE(1, { 1,2,3,4 }));
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	system("pause");
	return 0;
}

