#pragma once

#include "cjw.hpp"
#include "cjw_optional.hpp"

#ifndef _mysql_h
#error "б╞(д├бузебу ;)д├ : required mysql.h"
#endif

struct MyTableValue {
	unsigned long long number;
	std::vector<std::string> key;
	std::vector<std::vector<std::string>> value;
};

class MySQL {
private:
	MYSQL mMySQL;

public:
	//const MyTableValue &ret = res;
	MySQL(char * host, char * user, char * pw, char * db, int port = 0, char * character = "utf8") {
		if (0 != mysql_library_init(0, NULL, NULL))
			_ERROR_(MYSQL_LIBRARY_INIT_ERROR);
		if (NULL == mysql_init(&this->mMySQL))
			_ERROR_(MYSQL_INIT_ERROR);
		if (NULL == mysql_real_connect(&this->mMySQL, host, user, pw, db, port, NULL, 0))
			_ERROR_(MYSQL_REAL_CONNECT_ERROR);
		if (0 != mysql_set_character_set(&this->mMySQL, character))
			_ERROR_(MYSQL_SET_CHARSET_NAME_ERROR);
	}

	bool WriteData(const char *query) {
		if (0 == mysql_query(&this->mMySQL, query))
			return true;
		return false;
	}

	Optional<MyTableValue> SelectData(const char *query) {
		MyTableValue res;
		if (0 == mysql_query(&this->mMySQL, query)) {
			MYSQL_RES *result = mysql_store_result(&this->mMySQL);
			res.number = mysql_num_rows(result);

			unsigned int fieldcount = mysql_num_fields(result);
			for (unsigned int i = 0; i < fieldcount; i++)
				res.key.push_back(mysql_fetch_field_direct(result, i)->name);

			MYSQL_ROW row = NULL;
			while (NULL != (row = mysql_fetch_row(result))) {
				std::vector<std::string> value;
				for (unsigned int i = 0; i < fieldcount; i++)
					value.push_back(row[i] == NULL ? "" : row[i]);
				res.value.push_back(value);
			}

			mysql_free_result(result);
			return res;
		}
		return Optional<MyTableValue>();
	}

	~MySQL() {
		mysql_close(&this->mMySQL);
		mysql_library_end();
	}
};


