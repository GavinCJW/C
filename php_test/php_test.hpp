#pragma once
#define ZEND_WIN32
#define PHP_WIN32
#define ZEND_DEBUG 0

#ifndef PHP_TEST_H
#define PHP_TEST_H

#ifdef PHP_WIN32  
#define PHP_TEST_API __declspec(dllexport)
#define _STATIC_ASSERT(expr) typedef char __static_assert_t[ (expr)?(expr):1 ]  
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_PHPTest_API __attribute__ ((visibility("default")))
#else  
#define PHP_PHPTest_API
#define _STATIC_ASSERT(expr) typedef char __static_assert_t[ (expr) ]  
#endif  

//线程安全
#ifdef ZTS
#include "TSRM.h"
#define TEST_G(v) TSRMG(test_globals_id, zend_test_globals *, v)
#else
#define TEST_G(v) (test_globals.v)
#endif

#ifndef PHP_TEST_VERSION
#define PHP_TEST_VERSION NO_VERSION_YET
#endif 

extern "C" {
#include "zend_config.w32.h"
#include "php.h"
#include "ext/standard/info.h"
}

PHP_MINIT_FUNCTION(test);
PHP_MSHUTDOWN_FUNCTION(test);
PHP_RINIT_FUNCTION(test);
PHP_RSHUTDOWN_FUNCTION(test);
PHP_MINFO_FUNCTION(test);

// PHP_FUNCTION  只用来声明函数的名称，置于函数的参数将在cpp中定义 
PHP_FUNCTION(test_base);
PHP_FUNCTION(test_array);
PHP_FUNCTION(test_file);

#endif/* PHP_TEST_MAIN_H*/