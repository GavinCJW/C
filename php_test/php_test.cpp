// �������µĺ궨�����ڱ�������лᷢ����error C2466: ���ܷ��䳣����СΪ0 ������Ĵ���
//#define ZTS //�߳���� #undef ZTS
#define PHP_COMPILER_ID "VC14" //����汾

#include <vector>
#include <string>
#include "php_test.hpp"
#include "php_util.hpp"
using namespace std;

zend_function_entry test_functions[] = {
	PHP_FE(test_base, NULL)
	PHP_FE(test_array, NULL)
	PHP_FE(test_file, NULL)
	PHP_FE_END
};

extern zend_module_entry test_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"test",
	test_functions,
	PHP_MINIT(test),
	PHP_MSHUTDOWN(test),
	PHP_RINIT(test),
	PHP_RSHUTDOWN(test),
	PHP_MINFO(test),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_TEST_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(test);

PHP_MINIT_FUNCTION(test){
	return SUCCESS;
}
PHP_MSHUTDOWN_FUNCTION(test){
	return SUCCESS;
}
PHP_RINIT_FUNCTION(test){
	return SUCCESS;
}
PHP_RSHUTDOWN_FUNCTION(test){
	return SUCCESS;
}
PHP_MINFO_FUNCTION(test){
	php_info_print_table_start();
	php_info_print_table_header(2, "test support", "enabled");
	php_info_print_table_end();
}

PHP_FUNCTION(test_base){
	char *content = NULL;    
	int content_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &content, &content_len) == FAILURE)
		return;
	if (content){
		string strRet = content;
		RETURN_STRING((char*)strRet.c_str(), 1);
	}else{
		php_error(E_WARNING, "init_module: content is NULL");
	}
}

PHP_FUNCTION(test_array){
	zval *arr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE)
		RETURN_NULL();

	auto arr_hash = Z_ARRVAL_P(arr);
	auto ret = ZEND_ARRAY_TO_MAP(Z_ARRVAL_P(arr));
	if (ret.empty()) {
		php_error(E_WARNING, "array key => value must be STRING , LONG , DOUBLE");
		RETURN_NULL();
	}
	array_init_size(return_value, ret.size());
	for (auto map : ret) {
		for (auto val : map) {
			/*zval temp;
			ZVAL_STRING(&temp , val.first.data());
			zend_hash_update(Z_ARRVAL_P(return_value), strpprintf(0, val.second.data()), &temp);*/
			add_assoc_string(return_value, val.second.data(), (char *)val.first.data());
		}
	}
	
	/*
	auto array_walk = [](zval *ele TSRMLS_DC) {
		zval temp = *ele; // ��ʱzval������convert_to_string ��ȾԭԪ��
		zval_copy_ctor(&temp);  // ������ zval �ռ䲢���� ele ��ֵ
		convert_to_string(&temp); // �ַ�������ת��
								  //�򵥵Ĵ�ӡ
		PHPWRITE(Z_STRVAL(temp), Z_STRLEN(temp));
		php_printf("\n");
		zval_dtor(&temp); //�ͷ���ʱ�� temp
		return ZEND_HASH_APPLY_KEEP;
	};
	zend_hash_apply(arr_hash, (apply_func_t)array_walk TSRMLS_CC);
	*/
}

PHP_FUNCTION(test_file)
{
	zval val;
	array_init(&val);
	add_assoc_string(&val, "name", "taek-007");

	RETURN_ZVAL(&val, 1, 0);
}
