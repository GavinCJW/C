#pragma once

#include <mysql.h>
#include <vector>

#define MYSQL_LIBRARY_INIT_ERROR -1 //初始化数据库  
#define MYSQL_INIT_ERROR -2 //初始化数据结构  
#define MYSQL_SET_CHARSET_NAME_ERROR -3 //设置字符集
#define MYSQL_REAL_CONNECT_ERROR -4 //连接数据库

struct MyTableValue {
	unsigned long long number;
	std::vector<std::string> key;
	std::vector<std::vector<std::string>> value;
};

class MySQL {
private:
	MYSQL mMySQL;
	MyTableValue res;

public:
	MySQL() {
		if (0 != mysql_library_init(0, NULL, NULL))
			exit(MYSQL_LIBRARY_INIT_ERROR);
		if (NULL == mysql_init(&this->mMySQL))
			exit(MYSQL_INIT_ERROR);
		//...
	}

	void connect(const char * host, const char * user, const char * pw, const char * db, int port = 0, const char * character = "utf8") {
		if (NULL == mysql_real_connect(&this->mMySQL, host, user, pw, db, port, NULL, 0))
			exit(MYSQL_REAL_CONNECT_ERROR);
		if (0 != mysql_set_character_set(&this->mMySQL, character))
			exit(MYSQL_SET_CHARSET_NAME_ERROR);
	}

	bool writeData(const char *query) {
		if (0 == mysql_query(&this->mMySQL, query))
			return true;
		return false;
	}

	bool selectData(const char *query) {
		if (0 == mysql_query(&this->mMySQL, query)) {
			MYSQL_RES *result = mysql_store_result(&this->mMySQL);
			this->res.number = mysql_num_rows(result);

			unsigned int fieldcount = mysql_num_fields(result);
			for (unsigned int i = 0; i < fieldcount; i++)
				this->res.key.push_back(mysql_fetch_field_direct(result, i)->name);

			MYSQL_ROW row = NULL;
			while (NULL != (row = mysql_fetch_row(result))) {
				std::vector<std::string> value;
				for (unsigned int i = 0; i < fieldcount; i++)
					value.push_back(row[i] == NULL ? "" : row[i]);
				this->res.value.push_back(value);
			}

			mysql_free_result(result);
			return true;
		}
		return false;
	}

	MyTableValue get() { return this->res; }

	~MySQL() {
		mysql_close(&this->mMySQL);
		mysql_library_end();
	}
}; 
