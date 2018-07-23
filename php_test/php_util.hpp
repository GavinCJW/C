#pragma once

#include <vector>
#include <string>
#include <map>
template<class T>
bool IN_TYPE(T key, std::vector<T> val) {
	for each (auto var in val) {
		if (key == var)
			return true;
	}
	return false;
};

#define MAP std::vector<std::map<std::string, std::string>>
/*value must be {STRING LONG DOUBLE}*/
MAP  ZEND_ARRAY_TO_MAP(zend_array *arr) {
	MAP ret;
	auto array_count = zend_hash_num_elements(arr);
	for (uint32_t i = 0; i < array_count; i++) {
		auto val = arr->arData[i].val;
		auto key = arr->arData[i].key;
		if (IN_TYPE(Z_TYPE_P(&val), { IS_STRING,IS_LONG,IS_DOUBLE })) {
			zval_copy_ctor(&val);  
			convert_to_string(&val); 
			std::map<std::string, std::string> map;
			map.insert(std::make_pair(ZSTR_VAL(key), Z_STRVAL(val)));
			ret.push_back(map);
		}else {
			ret.swap(MAP());
			break;
		}
		zval_dtor(&val);
	}
	return ret;
}