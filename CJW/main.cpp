#include <iostream>
#include "cjw.hpp"
#include "cjw_transcode.hpp"
#include "cjw_des.hpp"


int main() {
	
	std::string encrypt = "12345";
	std::cout << Converter::BTH(DES::MD5(encrypt));


	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	system("pause");
	return 0;
}

