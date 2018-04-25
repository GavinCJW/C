#pragma once
#pragma warning( disable : 4996 )
#pragma warning( disable : 4091 )

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  
#include <mysql.h>
#include <windows.h>   
#include <vector>
#include <memory>
#include <string>
#include <ctime>
#include <fstream>

#if defined(__clang__)
#define CLANG
	/* Clang/LLVM. ---------------------------------------------- */
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#define ICC
	/* Intel ICC/ICPC. ------------------------------------------ */
#elif defined(__GNUC__) || defined(__GNUG__)
#define GNUC
	/* GNU GCC/G++. --------------------------------------------- */
#elif defined(__HP_cc) || defined(__HP_aCC)
	/* Hewlett-Packard C/aC++. ---------------------------------- */
#elif defined(__IBMC__) || defined(__IBMCPP__)
#define IBM
	/* IBM XL C/C++. -------------------------------------------- */
#elif defined(_MSC_VER)
#define MSVC
	/* Microsoft Visual Studio. --------------------------------- */
#elif defined(__PGI)
#define PGI
	/* Portland Group PGCC/PGCPP. ------------------------------- */
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#define OSS
	/* Oracle Solaris Studio. ----------------------------------- */
#else
	/* Error, both can't be defined or undefined same time */
#endif

/*Exception Code*/
#define MYSQL_LIBRARY_INIT_ERROR "MYSQL_LIBRARY_INIT_ERROR" //初始化数据库  
#define MYSQL_INIT_ERROR "MYSQL_INIT_ERROR" //初始化数据结构  
#define MYSQL_SET_CHARSET_NAME_ERROR "MYSQL_SET_CHARSET_NAME_ERROR" //设置字符集
#define MYSQL_REAL_CONNECT_ERROR "MYSQL_REAL_CONNECT_ERROR" //连接数据库
	
#define HTTP_SOKET_ERROR "HTTP_SOKET_ERROR"
#define HTTP_CONNECT_ERROR "HTTP_CONNECT_ERROR"
#define HTTP_SEND_ERROR "HTTP_SEND_ERROR"

#define  ENCRYPT_DATA_ERROR "ENCRYPT_DATA_ERROR"

#define _ERROR_(E) Exception::ExceptionHandler(E)

#ifndef EXCEPTION
#define EXCEPTION
class Exception :public std::exception {
private:
	const char * info;

public:
	inline Exception() = delete;
	inline explicit Exception(const char * info) noexcept :info(info) {}
	inline static Exception make(const char * info) { return Exception(info); }

	static bool ExceptionHandler(void* args) noexcept {
		auto t = time(nullptr);
		char date[20];
		strftime(date, 20, "%F %H:%M:%S", localtime(&t));
		try {
			throw Exception::make((const char *)args);
		}
		catch (...) {
			std::ofstream fout(".log", std::ios_base::app);
			fout << "ERROR_:" << (const char *)args << "\t\t" << "TIME_:" << date << "\n";
			exit(0);
		}
		return false;
	}
};
#endif

template<typename T>
inline std::istream &operator >> (std::istream &is, std::vector<T> &a) {
	auto rax = T();
	is >> rax;
	a.push_back(rax);
	return is;
}


	



