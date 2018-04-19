#pragma once

#include"cjw.hpp"
#ifdef MSVC

#include <WinBase.h>
#include <DbgHelp.h>
#pragma comment (lib ,"dbghelp.lib")

#define STACK_LENGTH 255
std::vector<DWORD64> _CAPTURESTACKTRACE_() {
	void* rax[STACK_LENGTH];
	const auto r1x = CaptureStackBackTrace(0, STACK_LENGTH, rax, NULL);
	std::vector<DWORD64> ret(r1x - 1);
	for (int i = 0; i < ret.size(); i++)
		ret[i] = DWORD64(rax[i + 1]);
	return std::move(ret);
}

std::shared_ptr<SYMBOL_INFO*> _SYMFROMADDR_(std::vector<DWORD64> address) {
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	std::shared_ptr<SYMBOL_INFO*> ret(new SYMBOL_INFO*[address.size()], [=](SYMBOL_INFO** ret) {
		for (int i = 0; i < address.size(); i++) 
			delete[] ret[i];
		delete[] ret;
	});
	
	for (int i = 0; i < address.size(); i++) {
		ret.get()[i] = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
		ret.get()[i]->MaxNameLen = 255;
		ret.get()[i]->SizeOfStruct = sizeof(SYMBOL_INFO);
		SymFromAddr(process, (address[i]), 0, ret.get()[i]);
	}
	return std::move(ret);
}

#endif