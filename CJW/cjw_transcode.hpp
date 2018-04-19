#pragma once

#include"cjw.hpp"
#include <windows.h>   


class Unicode {
private:

public:
	static std::string UTG(std::string utf8) {
		wchar_t * wstr = new wchar_t[MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, NULL, 0) + 1];
		MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, wstr, MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, NULL, 0));
		char *str = new char[WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL) + 1];
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL), NULL, NULL);
		utf8 = str;
		delete (wstr);
		delete (str);
		return utf8;
	}

	static std::string GTU(std::string gb2312)	{
		wchar_t* wstr = new wchar_t[MultiByteToWideChar(CP_ACP, 0, gb2312.data(), -1, NULL, 0) + 1];
		MultiByteToWideChar(CP_ACP, 0, gb2312.data(), -1, wstr, MultiByteToWideChar(CP_ACP, 0, gb2312.data(), -1, NULL, 0));
		char* str = new char[WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL) + 1];
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL), NULL, NULL);
		gb2312 = str;
		delete (wstr);
		delete (str);
		return gb2312;
	}

};

class Base64 {
private:
	
public:
	static std::string Encode(std::string str) {
		static const std::string _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		std::string _encode_result;
		unsigned long long bytes = str.length();
		const unsigned char * current = (unsigned char *)str.data();

		while (bytes > 2) {
			_encode_result += _base64_table[current[0] >> 2];
			_encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			_encode_result += _base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
			_encode_result += _base64_table[current[2] & 0x3f];
			current += 3;
			bytes -= 3;
		}
		if (bytes > 0){
			_encode_result += _base64_table[current[0] >> 2];
			if (bytes % 3 == 1) {
				_encode_result += _base64_table[(current[0] & 0x03) << 4];
				_encode_result += "==";
			}
			else if (bytes % 3 == 2) {
				_encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
				_encode_result += _base64_table[(current[1] & 0x0f) << 2];
				_encode_result += "=";
			}
		}
		return _encode_result;
	}

	static std::string Decode(std::string str){
		const char DecodeTable[] ={
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			62, // '+'
			0, 0, 0,
			63, // '/'
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
			0, 0, 0, 0, 0, 0, 0,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
			0, 0, 0, 0, 0, 0,
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
		};
		const char* current = str.data();
		unsigned long long bytes = str.length();
		std::string _decode_result;
		int nValue;
		for (unsigned long long i = 0; i < bytes;i+=4) {
				nValue =(DecodeTable[current[i]] << 18)+(DecodeTable[current[i+1]] << 12);
				_decode_result += (nValue & 0x00FF0000) >> 16;
				if (current[i+2] != '='){
					_decode_result += ((nValue += DecodeTable[current[i + 2]] << 6) & 0x0000FF00) >> 8;
					if (current[i+3] != '=')
						_decode_result += (nValue += DecodeTable[current[i + 3]]) & 0x000000FF;
				}
		}
		return _decode_result;
	}
};