#pragma once
#include "cjw.hpp"

class SHA {
public:
	static std::string SHA1(std::string message) {
		unsigned H[5] = { 0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0 }, Message_Digest[5]{0};
		unsigned Length_Low = 0, Length_High = 0 , Message_Block_Index =0;
		unsigned char Message_Block[64];    // 512-bit message blocks
		bool Corrupted = false;
		// 处理接下来512位数据的信息
		auto ProcessMessageBlock = [&]() {
			// SHA-1 常量
			const unsigned K[] = { 0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6 };
			unsigned list[80], buffer[5];

			//循环左移位
			auto CircularShift = [&](int bits, unsigned word) {
				return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32 - bits));
			};

			auto Operation = [&](int type, int pos, unsigned &A, unsigned &B, unsigned &C, unsigned &D, unsigned &E) {
				auto temp = 0;
				switch (type) {
				case 0:
					temp = (CircularShift(5, A) + ((B & C) | (~B & D)) + E + list[pos] + K[0]);
					break;
				case 1:
					temp = CircularShift(5, A) + (B ^ C ^ D) + E + list[pos] + K[1];
					break;
				case 2:
					temp = CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + list[pos] + K[2];
					break;
				case 3:
					temp = CircularShift(5, A) + (B ^ C ^ D) + E + list[pos] + K[3];
					break;
				default:
					break;
				}
				temp &= 0xFFFFFFFF;
				E = D;	D = C;	C = CircularShift(30, B);	B = A;	A = temp;
				return;
			};

			for (int i = 0; i < 16; i++)
				list[i] = ((unsigned)Message_Block[i * 4]) << 24
				| ((unsigned)Message_Block[i * 4 + 1]) << 16
				| ((unsigned)Message_Block[i * 4 + 2]) << 8
				| ((unsigned)Message_Block[i * 4 + 3]);

			for (int i = 16; i < 80; i++)
				list[i] = CircularShift(1, list[i - 3] ^ list[i - 8] ^ list[i - 14] ^ list[i - 16]);

			for (int i = 0; i < 5; i++)
				buffer[i] = H[i];

			for (int i = 0; i < 80; i++) {
				auto type = i / 20;
				Operation(type, i, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
			}

			for (int i = 0; i < 5; i++)
				H[i] = (H[i] + buffer[i]) & 0xFFFFFFFF;

			Message_Block_Index = 0;
		};
		//填充数据至512位
		auto PadMessage = [&]() {
			Message_Block[Message_Block_Index] = 0x80;
			if (Message_Block_Index++ > 55) {
				while (Message_Block_Index < 64)
					Message_Block[Message_Block_Index++] = 0;
				ProcessMessageBlock();
			}
			while (Message_Block_Index < 56)
				Message_Block[Message_Block_Index++] = 0;

			//将数据长度储存至最后八字节
			for (int i = 0, j = 24; i < 4; i++, j -= 8) {
				Message_Block[56 + i] = (Length_High >> j) & 0xFF;
				Message_Block[60 + i] = (Length_Low >> j) & 0xFF;
			}
			ProcessMessageBlock();
		};

		auto str = (const unsigned char*)message.c_str();
		while (*str) {
			Message_Block[Message_Block_Index++] = (*str & 0xFF);
			Length_Low = (Length_Low + 8) & 0xFFFFFFFF;
			if (Length_Low == 0) {
				Length_High = (Length_High + 1) & 0xFFFFFFFF;
				if (Length_High == 0) 
					_ERROR_(ENCRYPT_DATA_ERROR);
			}
			if (Message_Block_Index == 64)
				ProcessMessageBlock();
			str++;
		}
		PadMessage();
		//将主机数转换成无符号长整型的网络字节顺序，（将可能以小端储存的主机数换成大端存储）
		for (int i = 0; i < 5; i++)
			Message_Digest[i] = htonl(H[i]);

		return std::string((char *)reinterpret_cast<const unsigned char*>(Message_Digest)).substr(0, 20);
	}
};
