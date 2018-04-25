#include <iostream>
#include "cjw.hpp"
#include "cjw_transcode.hpp"
#include "cjw_des.hpp"


int main() {
	
	std::string server_key = "123456";
	
	std::cout << std::hex << Converter::HTB(Converter::BTH( SHA::SHA1(server_key))) << std::endl;

	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	system("pause");
	return 0;
}

