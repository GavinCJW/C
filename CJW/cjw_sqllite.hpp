#include "cjw.hpp"
#include "cjw_optional.hpp"
extern "C"
{
#include "sqlite3.h"
};

struct MyTableValue {
	unsigned long long number;
	std::vector<std::map<std::string,std::string>> value;
};

class SqlLite {
private :
	sqlite3 *DB;

public:
	SqlLite(const char * db) {
		int nRes = sqlite3_open(db, &DB);
		if (nRes != SQLITE_OK)
			_ERROR_(SQLLITE_OPENDB_ERROR);
	}

	bool WriteData(std::string query) {
		char * msg;
		if (SQLITE_OK == sqlite3_exec(DB, ("begin;\n" + query + "\ncommit; ").data(), 0, 0,&msg))
			return true;
		_ERROR_(SQLLITE_WRITE_ERROR);
		return false;
	}

	Optional<MyTableValue> SelectData(std::string query) {
		char * msg;
		MyTableValue ret;
		ret.number = 0;
		auto result = [](void *data, int argc, char **argv, char **col) {
			std::map<std::string, std::string> map;
			for (int i = 0; i < argc; i++)
				map[col[i]] =  argv[i] ? argv[i] : "NULL";
			
			auto ret = (MyTableValue *)data;
			ret->value.push_back(map);
			ret->number++;
			return 0;
		}; 
		if (SQLITE_OK == sqlite3_exec(DB, query.data(), result, (void*)&ret, &msg)) 
			return ret;		
		_ERROR_(SQLLITE_WRITE_ERROR);
		return Optional<MyTableValue>();
	}

	~SqlLite() {
		sqlite3_close(DB);
	}


};